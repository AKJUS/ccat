
# CountCAT
Cat out at given offset with given count/length in different formats

## Why, and for what i need it
I have huge json and other text files and i have to look sometimes is  
something wrong with this files. For example json reader raised exceptions  
and so on. And then i have to look why. And for this case i wrote this  
small tool.  
I know i can use all existing great tools like sed and awk and so on. But  
sometimes this great command line tools are a little oversized for me and  
and i like to code in C++. So why not to implement a tiny tool like this  
to solve this task.

## Looks like
[![Watch the video](https://i.ytimg.com/vi/NIph9nzp0z4/hqdefault.jpg)](https://www.youtube.com/watch?v=NIph9nzp0z4)

## How to use

- **ccat** -o:7546 -a:10 -c:bold tst.txt
- .
- Param **-o:** Offset of file which should printed out
- Param **-a:** How many chars before and after should printed out too
- Param **-c:** Color to mark char at offset, bold, red, green or blue
- Param **-tc:** Columncount for hextable
- Param **-HEX** Plain hex output
- Param **-HTABLE** Hextable output
- Param **-RAW** Plain text output
- .
- FilePath: Path to file you want printed out
