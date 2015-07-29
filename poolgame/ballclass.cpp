
class ball
{
    int x,y,number,radius;
    float d,v;
    public:
        ball(int,int);
}

ball::ball(int n,int r)
{
    number = n;
    radius = r;
    d = 0.0;
    v = 0.0;
}
