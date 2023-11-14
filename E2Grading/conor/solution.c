#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <pwd.h>
#include <string.h>

// TODO Tidy/shorten code

typedef struct
{
    int test_number;
    int tests_passed;
    int tests_failed;
} TestStats;

// TODO check name for every student submission and adjust accordingly
// #define FILENAME "./test.txt"
#define FILENAME "/proc/kdlp"

void print_test_summary(TestStats *stats)
{
    printf("\nTEST SUMMARY:\n");
    printf("--------------\n");
    printf("Total Tests: %d\nPassed: %d\nFailed: %d\n", stats->test_number - 1, stats->tests_passed, stats->tests_failed);
}

void check_file_owner(struct passwd *pwd, TestStats *stats)
{
    if (strcmp(pwd->pw_name, "root") == 0)
    {
        printf("Test %d passed: " FILENAME " owner is root as expected\n", stats->test_number);
        stats->tests_passed++;
    }

    else
    {
        printf("Test %d failed: Expected " FILENAME " owner is root, but owner is: %s\n", stats->test_number, pwd->pw_name);
        stats->tests_failed++;
    }

    stats->test_number++;
}

void check_file_permissions(struct stat file_stat, TestStats *stats)
{
    if ((file_stat.st_mode & 0777) == 0444)
    {
        printf("Test %d passed: " FILENAME " has 0444 permissions as expected\n", stats->test_number);
        stats->tests_passed++;
    }

    else
    {
        printf("Test %d failed: Expected " FILENAME " permissions to be 0444, but got: %o\n", stats->test_number, file_stat.st_mode & 0777);
        stats->tests_failed++;
    }

    stats->test_number++;
}

void get_file_info(const char *file_name, TestStats *stats)
{
    struct stat file_stat;
    struct passwd *pwd;

    if (stat(file_name, &file_stat) == -1)
    {
        printf("Test %d failed: Expected " FILENAME " owner is root, but got: %s\n", stats->test_number, strerror(errno));
        stats->tests_failed++;
        stats->test_number++;

        printf("Test %d failed: Expected " FILENAME " permissions to be 0444, but got: %s\n", stats->test_number, strerror(errno));
        stats->tests_failed++;
        stats->test_number++;

        return;
    }

    pwd = getpwuid(file_stat.st_uid);
    printf("File Name: %s\n", file_name);

    check_file_owner(pwd, stats);
    check_file_permissions(file_stat, stats);
}

void file_exists(const char *filename, TestStats *stats)
{
    char expected_output[30] = FILENAME " exists";

    if (access(filename, F_OK) == 0)
    {
        printf("Test %d passed: %s\n", stats->test_number, expected_output);
        stats->tests_passed++;
    }

    else
    {
        printf("Test %d failed: Expected %s, but got: %s\n", stats->test_number, expected_output, strerror(errno));
        stats->tests_failed++;
    }

    stats->test_number++;
}

void file_readable(const char *filename, TestStats *stats)
{
    char expected_output[4100] = FILENAME " has read permissions";

    if (access(filename, R_OK) == 0)
    {
        printf("Test %d passed: %s\n", stats->test_number, expected_output);
        stats->tests_passed++;
    }

    else
    {
        printf("Test %d failed: Expected %s, but got: %s\n", stats->test_number, expected_output, strerror(errno));
        stats->tests_failed++;
    }

    stats->test_number++;
}

void file_writable(const char *filename, TestStats *stats)
{
    char expected_output[100] = FILENAME " doesn't have write permissions";

    if (access(filename, F_OK) != 0)
    {
        printf("Test %d failed: Expected %s, but got: %s\n", stats->test_number, expected_output, strerror(errno));
        stats->tests_failed++;
    }

    else if (access(filename, W_OK) == 0)
    {
        printf("Test %d failed: Expected %s, but file has write permissions\n", stats->test_number, expected_output);
        stats->tests_failed++;
    }

    else
    {
        printf("Test %d passed: %s\n", stats->test_number, expected_output);
        stats->tests_passed++;
    }

    stats->test_number++;
}

ssize_t succesful_read_file(const char *filename, char *buffer, size_t nbytes, TestStats *stats)
{
    int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        printf("Test %d failed: Expected succesful read of " FILENAME ", but got: %s\n", stats->test_number, strerror(errno));
        stats->tests_failed++;
        stats->test_number++;
        return -1;
    }

    ssize_t bytes_read = read(fd, buffer, nbytes);
    close(fd);

    if (bytes_read == -1)
    {
        printf("Test %d failed: Expected succesful read of " FILENAME ", but got: %s\n", stats->test_number, strerror(errno));
        stats->tests_failed++;
    }

    else
    {
        printf("Test %d passed: Read %ld bytes from " FILENAME ": ", stats->test_number, bytes_read);
        buffer[bytes_read] = '\0';
        printf("%s", buffer);

        stats->tests_passed++;
    }

    stats->test_number++;

    return bytes_read;
}

ssize_t unsuccesful_read_file(const char *filename, char *buffer, size_t nbytes, TestStats *stats)
{
    int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        printf("Test %d failed: Expected unsuccesful read of " FILENAME " into %s buffer, but got: %s\n", stats->test_number, buffer == NULL ? "NULL" : "Invalid", strerror(errno));
        stats->tests_failed++;
        stats->test_number++;
        return -1;
    }

    ssize_t bytes_read = read(fd, buffer, nbytes);
    close(fd);

    if (bytes_read == -1)
    {
        if (errno == EFAULT)
        {
            printf("Test %d passed: Correctly failed to read from " FILENAME " into %s buffer. Didn't Crash!\n", stats->test_number, buffer == NULL ? "NULL" : "Invalid");
            stats->tests_passed++;
        }
        else
        {
            printf("Test %d failed: Expected EFAULT when reading " FILENAME " into %s buffer, but got: %s\n", stats->test_number, buffer == NULL ? "NULL" : "Invalid", strerror(errno));
            stats->tests_failed++;
        }
    }

    else
    {
        printf("Test %d failed: Expected unsuccesful read of " FILENAME " into %s buffer, but read %ld bytes from " FILENAME ": ", stats->test_number, buffer == NULL ? "NULL" : "Invalid", bytes_read);
        buffer[bytes_read] = '\0';
        printf("%s", buffer);

        stats->tests_failed++;
    }

    stats->test_number++;

    return bytes_read;
}

ssize_t write_file(const char *filename, const char *buffer, size_t nbytes, TestStats *stats)
{
    int fd = open(filename, O_WRONLY);

    // If open fails due to EACCES / permission denied, it should pass. If it fails for a different reason, it shouldn't pass
    if (fd == -1)
    {
        if (errno == EACCES)
        {
            printf("Test %d passed: Correctly failed to open " FILENAME " for writing: %s\n", stats->test_number, strerror(errno));
            stats->tests_passed++;
        }
        else
        {
            printf("Test %d failed: Expected failure to open " FILENAME " for writing due to EACCESS, but got: %s\n", stats->test_number, strerror(errno));
            stats->tests_failed++;
        }

        stats->test_number++;
        return -1;
    }

    ssize_t bytes_writen = write(fd, buffer, nbytes);
    close(fd);

    // If write fails due to operation not permitted, pass. Else fail.
    if (bytes_writen == -1)
    {
        if (errno == EPERM)
        {
            printf("Test %d passed: Failed to write to " FILENAME ": %s\n", stats->test_number, strerror(errno));
            stats->tests_passed++;
        }
        else
        {
            printf("Test %d failed: Expected failure to write to " FILENAME " due to EPERM, but failed due to: %s\n", stats->test_number, strerror(errno));
            stats->tests_failed++;
        }
    }

    // If write is succesful, fail
    else
    {
        printf("Test %d failed: Expected failure to write to " FILENAME " but write succeeded\n", stats->test_number);
        stats->tests_failed++;
    }

    stats->test_number++;

    return bytes_writen;
}

int seek(const char *filename, off_t offset, int whence, TestStats *stats)
{
    int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        printf("Test %d failed: Tried to move " FILENAME " offset with %s and offset %ld, but got: %s\n", stats->test_number, (whence == SEEK_SET) ? "SEEK_SET" : (whence == SEEK_CUR) ? "SEEK_CUR"
                                                                                                                                                                                       : "SEEK_END",
               offset, strerror(errno));

        stats->tests_failed++;
        stats->test_number++;
        return -1;
    }

    off_t starting_position = lseek(fd, 0, whence);
    off_t expected_position = starting_position + offset;
    off_t new_position = lseek(fd, offset, whence);

    close(fd);

    // SEEK_CUR and SEEK_SET should pass on succuss or when the resulting file position would be negative and invalid argument is returned
    if (whence == SEEK_CUR || whence == SEEK_SET)
    {

        // if the resulting position would be negative and EINVAL is returned as expected, pass
        if (errno == EINVAL && expected_position < 0)
        {
            printf("Test %d passed: Correctly failed to move " FILENAME " file position from %ld to %ld with %s and offset %ld: %s\n", stats->test_number, starting_position, expected_position, (whence == SEEK_SET) ? "SEEK_SET" : "SEEK_CUR",
                   offset, strerror(errno));
            stats->tests_passed++;
        }
        // if the operation fails for reason other than expected, fail
        else if (new_position == -1)
        {
            printf("Test %d failed: Tried to move " FILENAME " file position from %ld to %ld with %s and offset %ld, but got: %s\n", stats->test_number, starting_position, expected_position, (whence == SEEK_SET) ? "SEEK_SET" : "SEEK_CUR",
                   offset, strerror(errno));
            stats->tests_failed++;
        }
        // If the operation succeeds, pass
        else
        {
            printf("Test %d passed: Successfully moved " FILENAME " file position from %ld to %ld with %s and offset %ld\n", stats->test_number, starting_position, expected_position, (whence == SEEK_SET) ? "SEEK_SET" : "SEEK_CUR",
                   offset);
            stats->tests_passed++;
        }
    }

    // SEEK_END should not be supported.
    if (whence == SEEK_END)
    {
        if (new_position == -1)
        {
            printf("Test %d passed: Correctly failed to move " FILENAME " file position with unsupported SEEK_END and offset %ld: %s\n", stats->test_number, offset, strerror(errno));
            stats->tests_passed++;
        }
        else
        {
            printf("Test %d failed: Expected unsuccessful move of " FILENAME " file position from %ld to %ld with unsupported SEEK_END and offset %ld, but offset was moved to %ld\n", stats->test_number, starting_position, expected_position, offset, new_position);
            stats->tests_failed++;
        }
    }

    stats->test_number++;

    return new_position;
}

int main(void)
{
    char *filename = FILENAME;
    char buffer[50];
    char *null_buffer = NULL;
    char *invalid_buffer = (char *)0xdeadbeef;
    char string_buffer[16] = "This won't work";

    TestStats stats = {1, 0, 0};

    get_file_info(filename, &stats);

    file_exists(filename, &stats);
    file_readable(filename, &stats);
    file_writable(filename, &stats); // should pass on fail. Permission denied.

    succesful_read_file(filename, buffer, sizeof(buffer), &stats);
    unsuccesful_read_file(filename, null_buffer, sizeof(null_buffer), &stats);       // passing null buffer. Should pass on fail but not crash
    unsuccesful_read_file(filename, invalid_buffer, sizeof(invalid_buffer), &stats); // passing invalid buffer. Should pass on fail but not crash

    write_file(filename, string_buffer, sizeof(string_buffer), &stats); // should pass on fail due to either permission denied or operation not permitted

    seek(filename, 5, SEEK_CUR, &stats);
    seek(filename, 5, SEEK_SET, &stats);
    seek(filename, 5, SEEK_END, &stats);    // should pass on fail. SEEK_END unsupported
    seek(filename, -5, SEEK_CUR, &stats);   // should pass on fail. Negtaive expected file posiiton
    seek(filename, -5, SEEK_SET, &stats);   // should pass on fail. Negative expected file position
    seek(filename, -5, SEEK_END, &stats);   // should pass on fail. SEEK_END unsupported.
    seek(filename, -100, SEEK_END, &stats); // should pass on fail. Negative expected file position

    print_test_summary(&stats);

    return 0;
}
