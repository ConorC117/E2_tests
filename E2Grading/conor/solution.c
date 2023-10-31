#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define FILENAME "/proc/kdlp"
#define PRE "file " FILENAME " "

void file_exists(const char *filename)
{
    if (access(filename, F_OK) == 0)
        printf(PRE " exists\n");
    else
        printf(PRE " doesn't exist\n");
}

void file_readable(const char *filename)
{
    if (access(filename, R_OK) == 0)
        printf(PRE " has read permissions\n");
    else
        printf(PRE " doesn't have read permissions\n");
}

void file_writable(const char *filename)
{
    if (access(filename, W_OK) == 0)
        printf(PRE " has write permissions\n");
    else
        printf(PRE " doesn't have write permissions\n");
}

ssize_t read_file(const char *filename, char *buffer, size_t nbytes)
{
    int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        printf("Failed to open " PRE "\n");
        return -1;
    }

    ssize_t bytes_read = read(fd, buffer, nbytes);
    if (bytes_read < 1)
    {
        printf("Failed to read from " PRE "\n");
        perror("read error");
        close(fd);
        return -1;
    }

    printf("Read %ld bytes from " PRE "\n", bytes_read);

    buffer[bytes_read] = '\0';

    printf("%s", buffer);

    close(fd);

    return bytes_read;
}

ssize_t write_file(const char *filename, const char *buffer, size_t nbytes)
{
    int fd = open(filename, O_WRONLY);
    if (fd == -1)
    {
        printf("Failed to open " PRE "\n");
        perror("open error");
        return -1;
    }

    ssize_t bytes_write = write(fd, buffer, nbytes);
    if (bytes_write < 1)
    {
        printf("Failed to write " PRE "\n");
        perror("write error");
        close(fd);
        return -1;
    }

    return bytes_write;
}

int seek_current(const char *filename)
{
    int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        printf("Failed to open " PRE "\n");
        return -1;
    }

    int new_position = lseek(fd, 5, SEEK_CUR);
    if (new_position < 1)
    {
        printf("Failed to change offset " PRE "\n");
        perror("seek error");
        close(fd);
        return -1;
    }

    if (new_position == 5)
    {
        printf("Successfully moved offset with SEEK_CUR\n");
    }

    close(fd);

    return new_position;
}

int seek_set(const char *filename)
{
    int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        printf("Failed to open " PRE "\n");
        return -1;
    }

    int new_position = lseek(fd, 5, SEEK_SET);
    if (new_position < 1)
    {
        printf("Failed to change offset " PRE "\n");
        perror("seek error");
        close(fd);
        return -1;
    }

    if (new_position == 5)
    {
        printf("Successfully moved offset with SEEK_SET\n");
    }

    close(fd);

    return new_position;
}

int seek_end(const char *filename)
{
    int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        printf("Failed to open " PRE "\n");
        return -1;
    }

    int new_position = lseek(fd, 5, SEEK_END);
    if (new_position < 1)
    {
        printf("Failed to change offset " PRE "\n");
        perror("seek error");
        close(fd);
        return -1;
    }

    if (new_position == 40)
    {
        printf("Successfully moved offset with SEEK_END\n");
    }

    close(fd);

    return new_position;
}

int seek_negative_current(const char *filename)
{
    int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        printf("Failed to open " PRE "\n");
        return -1;
    }

    int new_position = lseek(fd, -5, SEEK_CUR);
    if (new_position < 1)
    {
        printf("Failed to change offset " PRE "\n");
        perror("seek error");
        close(fd);
        return -1;
    }

    if (new_position == 5)
    {
        printf("Successfully moved offset with SEEK_CUR\n");
    }

    close(fd);

    return new_position;
}

int seek_negative_set(const char *filename)
{
    int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        printf("Failed to open " PRE "\n");
        return -1;
    }

    int new_position = lseek(fd, -5, SEEK_SET);
    if (new_position < 1)
    {
        printf("Failed to change offset " PRE "\n");
        perror("seek error");
        close(fd);
        return -1;
    }

    if (new_position == 5)
    {
        printf("Successfully moved offset with SEEK_SET\n");
    }

    close(fd);

    return new_position;
}

int seek_negative_end(const char *filename)
{
    int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        printf("Failed to open " PRE "\n");
        return -1;
    }

    int new_position = lseek(fd, -5, SEEK_END);
    if (new_position < 1)
    {
        printf("Failed to change offset " PRE "\n");
        perror("seek error");
        close(fd);
        return -1;
    }

    if (new_position == 40)
    {
        printf("Successfully moved offset with SEEK_END\n");
    }

    close(fd);

    return new_position;
}

int main(void)
{
    char *filename = FILENAME;
    char buffer[50];
    char *null_buffer = NULL;
    char *invalid_buffer = (char *)0xdeadbeef;
    char string_buffer[16] = "This won't work";

    file_exists(filename);
    file_readable(filename);
    file_writable(filename);

    read_file(filename, buffer, sizeof(buffer));
    read_file(filename, null_buffer, sizeof(null_buffer));       // Intentional fail but doesn't crash
    read_file(filename, invalid_buffer, sizeof(invalid_buffer)); // fail but doesn't crash

    write_file(filename, string_buffer, sizeof(string_buffer)); // fail

    seek_current(filename);
    seek_set(filename);
    seek_end(filename);              // fail
    seek_negative_current(filename); // fail
    seek_negative_end(filename);     // fail
    seek_negative_set(filename);     // fail

    return 0;
}
