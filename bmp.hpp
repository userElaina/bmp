// code by userElaina
#include<bits/stdc++.h>

#define LL long long
#define ULL unsigned long long
#define B unsigned char
#define pt putchar
#define gt getchar
#define register
// #define inline

#define fu(x) (~(x)+1)
#define mn(a,b) (a<b?a:b)
#define mx(a,b) (a>b?a:b)
#define ab(a) (a>=0?a:fu(a))
#define absub(a,b) ((a<b)?(b-a):(a-b))
#define lowbit(a) (a&fu(a))

const int WHITE8=0xff;
const int BLACK8=0x00;
const int WHITE24=0xffffff;
const int BLACK24=0x000000;

const int BMP_bfType=0x4d42;
const int BMP_bfReserved=0;
const int BMP_biPlanes=1;

const int BMPRGB_biCompression=0;
const int BMPRGB_biXPelsPerMeter=0;
const int BMPRGB_biYPelsPerMeter=0;
const int BMPRGB_biClrUsed=0;
const int BMPRGB_biClrImportant=0;

const int BMPRGB24_bfOffBits=0x36;
const int BMPRGB24_biSize=0x28;
const int BMPRGB24_biBitCount=0x18;

const int DEFAULT_X=-2147483646;
const int DEFAULT_Y=-2147483645;
const int DEFAULT_Z=-2147483644;
const int DEFAULT_R=-2147483643;
const int DEFAULT_G=-2147483642;
const int DEFAULT_B=-2147483641;
const int DEFAULT_A=-2147483640;


#if defined(__WINDOWS_) || defined(_WIN32)
    std::string nul="nul";
    std::string split_path="\\";
    std::string awk_qm="\"";
    std::string mkdir_p="mkdir ";
#else
    std::string nul="/dev/null";
    std::string split_path="/";
    std::string awk_qm="'";
    std::string mkdir_p="mkdir -p ";
#endif

inline int argb(int r,int g=DEFAULT_G,int b=DEFAULT_B,int a=0){
    if(a<0||a>0xff)
        throw ".argb: a must be in [0x00,0xff]";
    if(g==DEFAULT_G&&b==DEFAULT_B){
        if(r<0||r>0xffffff)
            throw ".argb: rgb must be in [0x000000,0xffffff]";
        if(!a)
            return r;
        b=r&0xff;
        g=(r>>8)&0xff;
        r=r>>16;
    }else{
        if(r<0||r>0xff||g<0||g>0xff||b<0||b>0xff)
            throw ".argb: r,g,b must be in [0x00,0xff]";
    }
    if(a){
        r+=a-(r*a+127)/255;
        g+=a-(g*a+127)/255;
        b+=a-(b*a+127)/255;
    }
    return r<<16|g<<8|b;
}

inline int distance(int l,int r){
    if(l<0||l>0xffffff||r<0||r>0xffffff)
        throw ".distance: l or r out of range";
    l^=r;
    return (l>>16)+((l>>8)&0xff)+(l&0xff);
}


inline int isbmp(std::string pth){
    if(pth.size()<4)
        return 0;
    if(pth[pth.size()-4]!='.')
        return 0;
    if(pth[pth.size()-3]!='b')
        return 0;
    if(pth[pth.size()-2]!='m')
        return 0;
    if(pth[pth.size()-1]!='p')
        return 0;
    return 1;
}

inline std::string bmp(std::string pth,int resizex=0,int resizey=0){
    FILE*f=fopen(pth.c_str(),"rb");
    if(!f)
        throw ".bmp: file not found";
    fclose(f);

    if(isbmp(pth))return pth;
    int flg=resizex&&resizey;
    if(!flg&&(resizex||resizey))
        throw ".bmp: resizex and resizey must be both 0 or both not 0";

    std::string rx=std::to_string(resizex),ry=std::to_string(resizey);
    std::string p2=pth+(flg?"."+rx+"x"+ry:"")+".bmp";
    std::string cmd="ffmpeg -y -i \""+pth+"\" "+(flg?"-vf scale="+rx+":"+ry+" ":"")+"\""+p2+"\" 1>nul 2>&1";
    system(cmd.c_str());

    return p2;
}


class BMPrgb24{
private:
    int _size;

public:
    int size,width,height;
    B*o=NULL;
    int*tag=NULL;
    int lastag=0;
    std::string read_path="",write_path=""; // must be BMP
    std::string origin_path="",save_path="";// can be PNG or others

    B head[BMPRGB24_bfOffBits]={
//         0,   1,   2,   3,       4,   5,   6,   7,       8,   9,   A,   B,       C,   D,   E,   F
/*0x00*/0x42,0x4d,0xff,0xff,    0xff,0xff,0x00,0x00,    0x00,0x00,0x36,0x00,    0x00,0x00,0x28,0x00,
//                [       bfSize        ]
/*0x10*/0x00,0x00,0xff,0xff,    0xff,0xff,0xff,0xff,    0xff,0xff,0x01,0x00,    0x18,0x00,0x00,0x00,
//                [       biWidth       ] [       biHeight      ]
/*0x20*/0x00,0x00,0xff,0xff,    0xff,0xff,0x00,0x00,    0x00,0x00,0x00,0x00,    0x00,0x00,0x00,0x00,
//                [     biSizeImage     ]
/*0x30*/0x00,0x00,0x00,0x00,    0x00,0x00
    };

    BMPrgb24(LL x,LL y){
        width=x;
        height=y;

        size=width*height;
        _size=size+(size<<1);

        const LL biSizeImage=_size+(width&3)*height;
        const LL xy=y<<(LL)32|x;
        const LL bfSize=biSizeImage+BMPRGB24_bfOffBits;

        auto _sethead=[this](LL reg,int l,int r){while(l<r)this->head[l++]=reg&0xff,reg>>=8;};
        _sethead(bfSize,0x02,0x06);
        _sethead(xy,0x12,0x1a);
        _sethead(biSizeImage,0x22,0x26);

        renew();
    }

    BMPrgb24(std::string pth,int x=0,int y=0){
        origin_path=pth;
        read_path=bmp(pth,x,y);
        FILE*f=fopen(read_path.c_str(),"rb");
        if(!f)
            throw "BMPrgb24: file not found";
        const int code=read(f);
        if(code)printf("Warning: %d \"%s\"\n",code,pth.c_str());
        fclose(f);
    }

    ~BMPrgb24(){
        del();
    }
    
    inline void del(){
        if(o)delete[]o;
        if(tag)delete[]tag;
    }

    inline void renew(){
        del();
        o=(B*)malloc(_size);
        white();
        tag=(int*)malloc(size<<2);
        memset(tag,0,size<<2);
    }
    inline void white(){memset(o,0xff,_size);}
    inline void black(){memset(o,0x00,_size);}

    inline int read(FILE*f){
        if(fread(head,1,BMPRGB24_bfOffBits,f)^BMPRGB24_bfOffBits)
            throw "BMPrgb24.read: file.size < 0x36";
        B*_p=head;
        auto _n=[&_p](){return *_p++;};
        auto _n2=[&_n](){return _n()|_n()<<8;};
        auto _n4=[&_n2](){return _n2()|_n2()<<16;};

        if(_n2()^BMP_bfType)
            throw "BMPrgb24.read: bfType!='BM' (const)";
        // 0x00:0x01
        // const 'BM'

        const int bfSize=_n4();
        // 0x02:0x06

        if(_n4()^BMP_bfReserved)
            throw "BMPrgb24.read: bfReserved!=0 (const)";
        // 0x06:0x0A
        // const 0x00

        if(_n4()^BMPRGB24_bfOffBits)
            throw "BMPrgb24.read: bfOffBits!=0x36 ((const))";
        // 0x0A:0x0E
        // For 24-rgb-bpps, bfOffBitshead = 0x36 (bit)
        
        if(_n4()^BMPRGB24_biSize)
            throw "BMPrgb24.read: biSize!=0x28 ((const))";
        // 0x0E:0x12
        // For 24-rgb-bpps, biSize = 0x28 (bit)

        width=_n4();
        // 0x12:0x16
        height=_n4();
        // 0x16:0x1A

        size=width*height;
        _size=size+(size<<1);

        const int width3=width+(width<<1);
        const int mo=width&3;

        if(bfSize^((width3+mo)*height+BMPRGB24_bfOffBits))
            throw "BMPrgb24.read: bfSize!=biWidth*biHeight*3+bfOffBits";

        if(_n2()^BMP_biPlanes)
            throw "BMPrgb24.read: biPlanes!=0x01 (const)";
        // 0x1A:0x1B
        if(_n2()^BMPRGB24_biBitCount)
            throw "BMPrgb24.read: biBitCount!=0x18 ((const))";
        // 0x1C:0x1D

        if(_n4()^BMPRGB_biCompression)
            throw "BMPrgb24.read: biCompression!=0x00 ((const))";
        // 0x1E:0x22
        // const compress 0x00

        if(bfSize^(_n4()+BMPRGB24_bfOffBits))
            throw "BMPrgb24.read: bfSize!=biSizeImage+bfOffBits";
        // 0x22:0x26

        if(_n4()^BMPRGB_biXPelsPerMeter)
            throw "BMPrgb24.read: biXPelsPerMeter!=0x00 ((const))";
        // 0x26:0x2A
        // const 0x00
        if(_n4()^BMPRGB_biYPelsPerMeter)
            throw "BMPrgb24.read: biYPelsPerMeter!=0x00 ((const))";
        // 0x2A:0x2E
        // const 0x00
        /*
        bug(?):
            biYPelsPerMeter = 0x1d87
            In Windows Paint
            Cannot be reproduced
        */

        if(_n4()^BMPRGB_biClrUsed)
            throw "BMPrgb24.read: biClrUsed!=0x00 ((const))";
        // 0x2E:0x32
        // const 0x00

        if(_n4()^BMPRGB_biClrImportant)
            throw "BMPrgb24.read: biClrImportant!=0x00 ((const))";
        // 0x32:0x36

        _p=(B*)malloc(4);
        renew();
        for(int i=0;i<height;i++){
            if(fread(o+i*width3,1,width3,f)^width3)
                throw "BMPrgb24.read: file.size < bfsize";
            if(fread(_p,1,mo,f)^mo)
                throw "BMPrgb24.read: file.size < bfsize";
        }

        if(fread(_p,1,1,f))
            return 1;
        return 0;
    }

    inline int save(std::string pth=""){
        if(pth.empty()){
            printf("BMPrgb24.save: File '%s' already exists. Overwrite? [y/N]",read_path.c_str());
            if(gt()^'y'){
                save_path="";
                write_path="";
                return 1;
            }
            pth=read_path;
        }
        save_path=pth;
        write_path=isbmp(pth)?pth:pth+".bmp";
        FILE*f=fopen(write_path.c_str(),"wb");
        const int width3=width+(width<<1);
        const int mo=width&3;
        B*_p=(B*)malloc(4);
        memset(_p,0,4);
        fwrite(head,1,BMPRGB24_bfOffBits,f);
        for(int i=0;i<height;i++){
            fwrite(o+i*width3,1,width3,f);
            fwrite(_p,1,mo,f);
        }
        fclose(f);
        if(isbmp(pth))return 0;
        
        std::string s="ffmpeg -y -f bmp_pipe -i \""+pth+".bmp\" \""+pth+"\" 1>nul 2>&1";
        system(s.c_str());
        return 0;
    }

    inline int getp(int x,const int y=DEFAULT_Y){
        if(y^DEFAULT_Y){
            if(x<0||y<0||x>=width||y>=height){
                return -1;
            }
            return x+y*width;
        }else{
            if(x<0||x>=size){
                return -1;
            }
            return x;
        }
    }

    inline int getag(const int x,const int y=DEFAULT_Y){
        const int p=getp(x,y);
        if(p<0){
            throw p;
        }

        return tag[p];
    }

    inline B getb(const int x,const int y=DEFAULT_Y){
        const int p=getp(x,y);
        if(p<0){
            throw p;
        }

        // o[p*3]
        return o[p+(p<<1)];
    }
    inline B getg(const int x,const int y=DEFAULT_Y){
        const int p=getp(x,y);
        if(p<0){
            throw p;
        }

        // o[p*3+1]
        return o[p+(p<<1|1)];
    }
    inline B getr(const int x,const int y=DEFAULT_Y){
        const int p=getp(x,y);
        if(p<0){
            throw p;
        }

        // o[p*3+2]
        return o[p+((p+1)<<1)];
    }

    inline int getpixel(const int x,const int y=DEFAULT_Y){
        const int p=getp(x,y);
        if(p<0){
            throw p;
        }

        int p3=p+(p<<1);
        const int b=o[p3++];
        const int g=o[p3++];
        const int r=o[p3];
        return r<<16|g<<8|b;
    }

    inline int setag(const int x,const int y=DEFAULT_Y,const int z=DEFAULT_Z){
        if(z^DEFAULT_Z){
            if(y^DEFAULT_Y){
                lastag=z;
            }else{
                throw "BMPrgb24.setag: Need 1 or 3 args";
            }
        }else{
            if(y^DEFAULT_Y){
                throw "BMPrgb24.setag: Need 1 or 3 args";
            }else{}
        }

        const int p=getp(x,y);
        if(p<0){
            throw p;
        }

        tag[p]=lastag;
        return 0;
    }

    inline int setpixel(const int x,const int y=DEFAULT_Y,int rgb=DEFAULT_A){
        if(rgb^DEFAULT_A){
            if(y^DEFAULT_Y){}else{
                throw "BMPrgb24.setpixel: Need 1 or 3 args";
            }
        }else{
            if(y^DEFAULT_Y){
                throw "BMPrgb24.setpixel: Need 1 or 3 args";
            }else{
                rgb=0x000000;
            }
        }

        const int p=getp(x,y);
        if(p<0){
            throw p;
        }

        const int b=rgb&0xff;
        const int g=(rgb>>8)&0xff;
        const int r=rgb>>16;
        int p3=p+(p<<1);
        o[p3++]=b;
        o[p3++]=g;
        o[p3]=r;
        return 0;
    }


    inline int bgray(const int rgb=0xffffff){
        const int b=rgb&0xff;
        const int g=(rgb>>8)&0xff;
        const int r=rgb>>16;
        for(int i=0;i<_size;){
            const int c=299*o[i+2]+587*o[i+1]+114*o[i];
            o[i++]=(c*b+127500)/255000;
            o[i++]=(c*g+127500)/255000;
            o[i++]=(c*r+127500)/255000;
        }
        return 0;
    }


    inline int wgray(const int rgb=0x000000){
        const int b=(rgb&0xff)^0xff;
        const int g=((rgb>>8)&0xff)^0xff;
        const int r=(rgb>>16)^0xff;
        for(int i=0;i<_size;){
            const int c=255000-(299*o[i+2]+587*o[i+1]+114*o[i]);
            o[i++]=((c*b+127500)/255000)^0xff;
            o[i++]=((c*g+127500)/255000)^0xff;
            o[i++]=((c*r+127500)/255000)^0xff;
        }
        return 0;
    }

    inline int gray(const int rgb=0x000000){
        const int b=rgb&0xff;
        const int g=(rgb>>8)&0xff;
        const int r=rgb>>16;
        // if((299*r+587*g+114*b)>128*1000)
        if((306*r+601*g+117*b)&131072)
            return bgray(rgb);
        else
            return wgray(rgb);
    }

    inline int linear(int con=0b111){
        return linear(0x000000,0xffffff,con);
    }

    inline int linear(int rgb_black1,int rgb_white1,int con=0b111){
        int b_max=0x00,g_max=0x00,r_max=0x00;
        int b_min=0xff,g_min=0xff,r_min=0xff;
        for(int i=0;i<_size;){
            b_max=b_max>o[i]?b_max:o[i];
            b_min=b_min<o[i]?b_min:o[i];
            i++;
            g_max=g_max>o[i]?g_max:o[i];
            g_min=g_min<o[i]?g_min:o[i];
            i++;
            r_max=r_max>o[i]?r_max:o[i];
            r_min=r_min<o[i]?r_min:o[i];
            i++;
        }
        int _min=r_min<<16|g_min<<8|b_min;
        int _max=r_max<<16|g_max<<8|b_max;
        return linear(_min,_max,rgb_black1,rgb_white1,con);
    }

    inline int _linearp(const int _o,const int _black0,const int _white0,const int _lv0,const int _black1,const int _white1,const int _lv1){
        if(!_lv0){
            return _o<_black0?_black1:_white1;
        }
        if(!_lv1){
            return _black1;
        }
        if(_o<=_black0){
            return _black1;
        }
        if(_o>=_white0){
            return _white1;
        }
        return (_o-_black0)*_lv1/_lv0+_black1;
    }

    inline int linear(int rgb_black0,int rgb_white0,int rgb_black1,int rgb_white1,int con=0b111){
        if(!con)return 1;

        const int b_black0=rgb_black0&0xff;
        const int g_black0=(rgb_black0>>8)&0xff;
        const int r_black0=rgb_black0>>16;

        const int b_white0=rgb_white0&0xff;
        const int g_white0=(rgb_white0>>8)&0xff;
        const int r_white0=rgb_white0>>16;

        const int b_lv0=b_white0-b_black0;
        const int g_lv0=g_white0-g_black0;
        const int r_lv0=r_white0-r_black0;

        const int b_black1=rgb_black1&0xff;
        const int g_black1=(rgb_black1>>8)&0xff;
        const int r_black1=rgb_black1>>16;

        const int b_white1=rgb_white1&0xff;
        const int g_white1=(rgb_white1>>8)&0xff;
        const int r_white1=rgb_white1>>16;

        const int b_lv1=b_white1-b_black1;
        const int g_lv1=g_white1-g_black1;
        const int r_lv1=r_white1-r_black1;

        for(int i=0;i<_size;){
            if(con&1){
                o[i]=_linearp(o[i],b_black0,b_white0,b_lv0,b_black1,b_white1,b_lv1);
            }
            i++;
            if(con&2){
                o[i]=_linearp(o[i],g_black0,g_white0,g_lv0,g_black1,g_white1,g_lv1);
            }
            i++;
            if(con&4){
                o[i]=_linearp(o[i],r_black0,r_white0,r_lv0,r_black1,r_white1,r_lv1);
            }
            i++;
        }
        return 0;
    }

    inline BMPrgb24*resize(LL x,LL y,int f=0){
        if(f==0){
            return resize_avg(x,y);
        }
        return this;
    }

    inline BMPrgb24*resize_avg(LL x,LL y){
        BMPrgb24*img=new BMPrgb24(x,y);
        for(int i=0;i<x;i++)for(int j=0;j<y;j++){
            LL r=0,g=0,b=0,check=0;

            const int il=i*width;
            const int ir=il+width-1;
            const int jl=j*height;
            const int jr=jl+height-1;

            const int xl=il/x;
            const int xr=ir/x;
            const int yl=jl/y;
            const int yr=jr/y;

            const int dxl=il-xl*x;
            const int dxr=(xr+1)*x-1-ir;
            const int dyl=jl-yl*y;
            const int dyr=(yr+1)*y-1-jr;

            LL r2,g2,b2,v,v2;

            // all
            v2=b2=g2=r2=0;
            for(int xi=xl;xi<=xr;xi++){
                for(int yj=yl;yj<=yr;yj++){
                    v2++;
                    const int p=getp(xi,yj);
                    b2+=getb(p);
                    g2+=getg(p);
                    r2+=getr(p);
                }
            }
            v=x*y;
            check+=v2*v;
            b+=b2*v;
            g+=g2*v;
            r+=r2*v;

            // left
            v2=b2=g2=r2=0;
            for(int yj=yl;yj<=yr;yj++){
                v2++;
                const int p=getp(xl,yj);
                b2+=getb(p);
                g2+=getg(p);
                r2+=getr(p);
            }
            v=fu(dxl*y);
            check+=v2*v;
            b+=b2*v;
            g+=g2*v;
            r+=r2*v;

            // right
            v2=b2=g2=r2=0;
            for(int yj=yl;yj<=yr;yj++){
                v2++;
                const int p=getp(xr,yj);
                b2+=getb(p);
                g2+=getg(p);
                r2+=getr(p);
            }
            v=fu(dxr*y);
            check+=v2*v;
            b+=b2*v;
            g+=g2*v;
            r+=r2*v;

            // up
            v2=b2=g2=r2=0;
            for(int xi=xl;xi<=xr;xi++){
                v2++;
                const int p=getp(xi,yl);
                b2+=getb(p);
                g2+=getg(p);
                r2+=getr(p);
            }
            v=fu(x*dyl);
            check+=v2*v;
            b+=b2*v;
            g+=g2*v;
            r+=r2*v;

            // down
            v2=b2=g2=r2=0;
            for(int xi=xl;xi<=xr;xi++){
                v2++;
                const int p=getp(xi,yr);
                b2+=getb(p);
                g2+=getg(p);
                r2+=getr(p);
            }
            v=fu(x*dyr);
            check+=v2*v;
            b+=b2*v;
            g+=g2*v;
            r+=r2*v;

            // left up
            const int pll=getp(xl,yl);
            b2=getb(pll);
            g2=getg(pll);
            r2=getr(pll);
            v=dxl*dyl;
            check+=v;
            b+=b2*v;
            g+=g2*v;
            r+=r2*v;

            // right up
            const int prl=getp(xr,yl);
            b2=getb(prl);
            g2=getg(prl);
            r2=getr(prl);
            v=dxr*dyl;
            check+=v;
            b+=b2*v;
            g+=g2*v;
            r+=r2*v;

            // left down
            const int plr=getp(xl,yr);
            b2=getb(plr);
            g2=getg(plr);
            r2=getr(plr);
            v=dxl*dyr;
            check+=v;
            b+=b2*v;
            g+=g2*v;
            r+=r2*v;

            // right down
            const int prr=getp(xr,yr);
            b2=getb(prr);
            g2=getg(prr);
            r2=getr(prr);
            v=dxr*dyr;
            check+=v;
            b+=b2*v;
            g+=g2*v;
            r+=r2*v;

            if(check^size){
                throw "BMPrgb24.resize_avg: check!=size";
            }

            img->setpixel(img->getp(i,j),argb((r+(size>>1))/size,(g+(size>>1))/size,(b+(size>>1))/size));
        }
        return img;
    }
};
