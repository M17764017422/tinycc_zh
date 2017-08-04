#���� <stdio.h>

/* Define architecture */
#�� �Ѷ���(__i386__) || �Ѷ��� _M_IX86
# ���� TRIPLET_ARCH "i386"
#���� �Ѷ���(__x86_64__) || �Ѷ��� _M_AMD64
# ���� TRIPLET_ARCH "x86_64"
#���� �Ѷ���(__arm__)
# ���� TRIPLET_ARCH "arm"
#���� �Ѷ���(__aarch64__)
# ���� TRIPLET_ARCH "aarch64"
#��
# ���� TRIPLET_ARCH "unknown"
#����

/* Define OS */
#�� �Ѷ��� (__linux__)
# ���� TRIPLET_OS "linux"
#���� �Ѷ��� (__FreeBSD__) || �Ѷ��� (__FreeBSD_kernel__)
# ���� TRIPLET_OS "kfreebsd"
#���� �Ѷ��� _WIN32
# ���� TRIPLET_OS "win32"
#���� !�Ѷ��� (__GNU__)
# ���� TRIPLET_OS "unknown"
#����

/* Define calling convention and ABI */
#�� �Ѷ��� (__ARM_EABI__)
# �� �Ѷ��� (__ARM_PCS_VFP)
#  ���� TRIPLET_ABI "gnueabihf"
# ��
#  ���� TRIPLET_ABI "gnueabi"
# ����
#��
# ���� TRIPLET_ABI "gnu"
#����

#�� �Ѷ��� _WIN32
# ���� TRIPLET TRIPLET_ARCH "-" TRIPLET_OS
#���� �Ѷ��� __GNU__
# ���� TRIPLET TRIPLET_ARCH "-" TRIPLET_ABI
#��
# ���� TRIPLET TRIPLET_ARCH "-" TRIPLET_OS "-" TRIPLET_ABI
#����

#�� �Ѷ���(_WIN32)
�� _CRT_glob = 0;
#����

�� main(�� argc, �� *argv[])
{
    ת��(argc == 2 ? argv[1][0] : 0) {
        ���� 'b':
        {
            �ױ� �޷� foo = 0x01234567;
            puts(*(�޷� ��*)&foo == 0x67 ? "no" : "yes");
            ����;
        }
#�綨�� __GNUC__
        ���� 'm':
            printf("%d\n", __GNUC_MINOR__);
            ����;
        ���� 'v':
            printf("%d\n", __GNUC__);
            ����;
#���� �Ѷ��� __TINYC__
        ���� 'v':
            puts("0");
            ����;
        ���� 'm':
            printf("%d\n", __TINYC__);
            ����;
#��
        ���� 'm':
        ���� 'v':
            puts("0");
            ����;
#����
        ���� 't':
            puts(TRIPLET);
            ����;

        ȱʡ:
            ����;
    }
    ���� 0;
}
