#include "LLauncher.h"
#include <cstdlib>
#include <cstring>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

static int pipeA[2] = {-1, -1};
static int pipeB[2] = {-1, -1};

void LLauncher::init()
{
    if (pipeA[0] != -1)
        return;

    pipe(pipeA);
    pipe(pipeB);

    pid_t child_pid = fork();

    // Child process
    if (child_pid == 0) {

        close(pipeA[1]);
        close(pipeB[0]);
        fcntl(pipeA[0], F_SETFD, fcntl(pipeA[0], F_GETFD) | FD_CLOEXEC);
        fcntl(pipeB[1], F_SETFD, fcntl(pipeB[1], F_GETFD) | FD_CLOEXEC);

        pollfd fds;
        fds.events = POLLIN;
        fds.revents = 0;
        fds.fd = pipeA[0];

        // Read messages from the parent
        std::string cmd =  "";
        char c;

        while (true)
        {
            if (poll(&fds, 1, -1) != 1)
                exit(1);

            int n = read(pipeA[0], &c, sizeof(c));

            if (n == 0)
                exit(1);

            while (n > 0)
            {
                if (c == '\0')
                {
                    int pid = fork();

                    if (pid == 0)
                    {
                        exit(system(cmd.c_str()));
                        return;
                    }
                    else
                    {
                        cmd = "";
                        char buffer[128];
                        sprintf(buffer, "%d", pid);
                        char *ptr = buffer;

                        while (true)
                        {
                            ssize_t w = write(pipeB[1], ptr, sizeof(c));

                            if (*ptr == '\0')
                                break;

                            if (w > 0)
                                ptr += w;
                        }

                        goto red;
                    }
                }
                else
                     cmd += c;

                red:
                n = read(pipeA[0], &c, sizeof(c));
            }
        }

        close(pipeA[0]);
        close(pipeB[1]);
        exit(EXIT_SUCCESS);
    }
    else
    {
        // Close the read end of the pipe in the parent
        close(pipeA[0]);
        close(pipeB[1]);
    }
}

int LLauncher::launch(const char *cmd)
{
    if (pipeA[0] == -1 || !cmd)
        return -1;

    pollfd fds;
    fds.events = POLLIN;
    fds.revents = 0;
    fds.fd = pipeB[0];
    char c;

    // Read prev message if any
    if (poll(&fds, 1, 0) == 1)
        while (read(pipeB[0], &c, 1) > 0) {}

    ssize_t s;
    while (true)
    {
        s = write(pipeA[1], cmd, 1);

        if (s < 0)
            return -1;

        if (s == 0)
            continue;

        if (*cmd == '\0')
            break;

        cmd += s;
    }

    std::string res = "";

    while (true)
    {
        if (poll(&fds, 1, 500) != 1)
            return -1;

        s = read(pipeB[0], &c, 1);

        if (s <= 0)
            return -1;

        if (c == '\0')
            return atoi(res.c_str());

        res += c;
    }

    return -1;
}
