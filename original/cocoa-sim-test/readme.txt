(1)请先配置编译环境:
MIPS-crosscompile
(ubuntu请安装类似的包)
 > yum install gmp-devel gmp mpfr mpfr-devel libmpc libmpc-devel


1)# env
export PREFIX=/opt/gccmips
export TARGET=mipsel-gnu-linux
export PATH=$PATH:/opt/gccmips/bin
# 在~/.bashrc中设置PATH一劳永逸

2)# binutils2.21:
 > ./configure --prefix=$PREFIX --target=$TARGET
 > make -j4
 > make install              # as root

3)# gcc
 > ./configure --prefix=$PREFIX --target=$TARGET
 > make -j4 all-gcc
 > make install-gcc          # as root


(2)编译测试代码
  > make
生成了"bin.bin"文件,二进制.
生成了"bin.elf.txt"文件,反汇编对照.

(3)加载并仿真
1)用写flash文件
 > cocoa-flash-ed
 cocoa-flash-ed > l c00000 bin.bin
 cocoa-flash-ed > q
2)运行模拟器
 > cocoa-sim-debug
  查看帮助:
 cocoa-sim-debug > h 

