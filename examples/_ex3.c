#���� <tcclib.h>

�� 쳲�����(�� ��)
{
    �� (�� <= 2)
        ���� 1;
    ��
        ���� 쳲�����(�� - 1) + 쳲�����(�� - 2);
}

�� main(�� argc, �� **argv)
{
    �� ��;
    �� (argc < 2) {
        printf("usage: fib n\n"
               "Compute nth Fibonacci number\n");
        ���� 1;
    }

    �� = atoi(argv[1]);
    printf("fib(%d) = %d\n", ��, 쳲�����(��));
    ���� 0;
}
