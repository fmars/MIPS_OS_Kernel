(1)�������ñ��뻷��:
MIPS-crosscompile
(ubuntu�밲װ���Ƶİ�)
 > yum install gmp-devel gmp mpfr mpfr-devel libmpc libmpc-devel


1)# env
export PREFIX=/opt/gccmips
export TARGET=mipsel-gnu-linux
export PATH=$PATH:/opt/gccmips/bin
# ��~/.bashrc������PATHһ������

2)# binutils2.21:
 > ./configure --prefix=$PREFIX --target=$TARGET
 > make -j4
 > make install              # as root

3)# gcc
 > ./configure --prefix=$PREFIX --target=$TARGET
 > make -j4 all-gcc
 > make install-gcc          # as root


(2)������Դ���
  > make
������"bin.bin"�ļ�,������.
������"bin.elf.txt"�ļ�,��������.

(3)���ز�����
1)��дflash�ļ�
 > cocoa-flash-ed
 cocoa-flash-ed > l c00000 bin.bin
 cocoa-flash-ed > q
2)����ģ����
 > cocoa-sim-debug
  �鿴����:
 cocoa-sim-debug > h 

