#include <SFML/Graphics.hpp>
#include <string>
#include <cmath>
#include <iostream>
#include <random>

#define PI 3.1415
#define RADIUS 13

using namespace std;

default_random_engine generator;
uniform_real_distribution<float> dist(0.0,1.0);

bool collisionCheck(float x1,float y1,float v1,float d1,float x2,float y2,float v2,float d2)
{
    float dis = (x1-x2)*(x1-x2)+(y1-y2)*(y1-y2);
    dis = sqrt(dis);
    dis-=2*RADIUS;
    float V[] = {v1*cos(d1)-v2*cos(d2),v1*sin(d1)-v2*sin(d2)};
    float mag = sqrt(V[0]*V[0]+V[1]*V[1]);
    if(dis>mag) return false;
    float C[] = {x2-x1,y2-y1};
    float dot = V[0]*C[0]+V[1]*C[1];
    if(dot<=0) return false;
    float N[] = {V[0]/mag,V[1]/mag};
    float D = N[0]*C[0]+N[1]*C[1];
    float F = C[0]*C[0]+C[1]*C[1]-D*D;
    if(F>(2*RADIUS)*(2*RADIUS)) return false;
    float T = (2*RADIUS)*(2*RADIUS)-F;
    float dtm = D-sqrt(T);
    if(dtm>mag) return false;
    return true;
}

bool wallCheck(float x,float y,float v,float &d,char &w)
{
    float dlw = x-RADIUS-52;
    float dtw = y-RADIUS-53;
    float drw = 912-(x+RADIUS);
    float dbw = 482-(y+RADIUS);
    if(dlw>v&&dtw>v&&drw>v&&dbw>v) return false;

    float walls[][2][2] =
    {
    {{52,87},{52,448}},
    {{86,53},{457,53}},
    {{505,53},{878,53}},
    {{912,87},{912,448}},
    {{505,482},{878,482}},
    {{86,482},{457,482}}
    };

    float V[] = {v*cos(d),v*sin(d)};
    bool hit=false;
    for(int wall=0;wall<6;wall++)
    {
        float wvec[] = {walls[wall][0][0]-walls[wall][1][0],walls[wall][0][1]-walls[wall][1][1]};
        float mag = sqrt(wvec[0]*wvec[0]+wvec[1]*wvec[1]);
        float uvec[] = {wvec[0]/mag,wvec[1]/mag};
        float C[] = {walls[wall][0][0]-x,walls[wall][0][1]-y};
        float cmag2 = C[0]*C[0]+C[1]*C[1];
        float ddp = C[0]*uvec[0]+C[1]*uvec[1];
        float btwd = sqrt(cmag2-ddp*ddp); // ball to wall distance
        if(wall==1||wall==2||wall==4||wall==5)
        {
            if(!(walls[wall][0][0]<x<walls[wall][1][0]))
            {
                if(wall==1||wall==5) btwd=sqrt((walls[wall][1][0]-x)*(walls[wall][1][0]-x)+(walls[wall][1][1]-y)*(walls[wall][1][1]-y));
                else btwd=sqrt((walls[wall][0][0]-x)*(walls[wall][0][0]-x)+(walls[wall][0][1]-y)*(walls[wall][0][1]-y));
            }
        }
        if(btwd>RADIUS) continue;
        cout<<btwd<<endl;
        //float angle1 = atan((walls[wall][0][1]-y)/(walls[wall][0][0]-x));
        //float angle2 = atan((walls[wall][1][1]-y)/(walls[wall][1][0]-x));

        //if(angle1<0) angle1+=2*PI;
        //if(angle2<0) angle2+=2*PI;
        //if(d<0) d+=2*PI;
        //cout<<angle1<<" "<<angle2<<" "<<d<<endl;
        //if((angle1<d<angle2)||(angle2<d<angle1))
        //{

        hit = true;
        if(wall==0||wall==3) w='x';
        else w='y';
        float cx[] = {55,483,907,907,483,55};
        float cy[] = {55,45,55,480,495,480};
        for(int i=0;i<6;i++)
            {
                if(sqrt((cx[i]-x)*(cx[i]-x)+(cy[i]-y)*(cy[i]-y))<RADIUS+3) w='p';
            }
        //}
    }
    if(!hit) return false;

    return true;
}

void madeball(vector<float> & x,vector<float> & y,vector<float> & v,vector<float> & d,vector<sf::Texture> &t,vector<int> &bn,int i)
{
    if(i==0)
    {
        x[i]=200;
        y[i]=267;
        v[i]=0;
        d[i]=0;
    }
    else
    {
        x.erase(x.begin()+i);
        y.erase(y.begin()+i);
        v.erase(v.begin()+i);
        d.erase(d.begin()+i);
        t.erase(t.begin()+i);
        bn.erase(bn.begin()+i);
    }
}

void setup(vector<float> & x,vector<float> & y,vector<float> & v,vector<float> & d,vector<sf::Texture> &t,vector<int> &bn)
{
    x.clear();
    y.clear();
    v.clear();
    d.clear();
    t.clear();
    bn.clear();

    float startx[] = {220.0,700.0,723.0,723.0,746.0,746.0,746.0,769.0,769.0,769.0,769.0,792.0,792.0,792.0,792.0,792.0};
    float starty[] = {247.0+40*dist(generator),267.0,281.0,253.0,295.0,267.0,239.0,309.0,281.0,253.0,225.0,323.0,295.0,267.0,239.0,211.0};
    float startv[] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
    float startd[] = {-PI/10+PI*dist(generator)/5,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
    int balltexture[] = {0,1,3,11,14,8,6,9,4,15,13,12,5,10,2,7};

    for(int i=0;i<16;i++)
    {
        x.push_back(startx[i]);
        y.push_back(starty[i]);
        v.push_back(startv[i]);
        d.push_back(startd[i]);
        sf::Texture texture;
        texture.loadFromFile(to_string(i)+".png");
        t.push_back(texture);
        bn.push_back(balltexture[i]);
    }
}

void moveStones(vector<float> & x,vector<float> & y,vector<float> & v,vector<float> & d)
{
    for(int i=0;i<x.size();i++)
    {
        x[i]+= cos(d[i])*v[i];
        y[i]+= sin(d[i])*v[i];
        v[i]-= .04;
        if(v[i]<.15) v[i]=0;
        if(x[i]>899||x[i]<67){
         if(!(y[i]>445||y[i]<90)) d[i]=PI-d[i];
        }
        if(y[i]>469||y[i]<69){
         if(!(x[i]>864||x[i]<85)) d[i]=2*PI-d[i];
         if(x[i]>464&&(x[i]<500)) d[i]=2*PI-d[i];
        }
    }
}

void addPolar(float t1, float r1, float t2, float r2,float & tf, float & rf)
{
    float x1 = cos(t1)*r1;
    float y1 = sin(t1)*r1;
    float x2 = cos(t2)*r2;
    float y2 = sin(t2)*r2;
    float xf = x1+x2;
    float yf = y1+y2;
    if(xf==0) xf=0.001;
    if(yf==0) yf=0.001;
    if(xf<0) tf = PI+atan(yf/xf);
    else tf = atan(yf/xf);
    rf = sqrt(xf*xf+yf*yf);
}

void hitwall(float &d,char w)
{
    if(w=='x') d=PI-d;
    if(w=='y') d*=-1;
}

void collide(float x1,float y1,float &v1,float &d1,float x2, float y2, float &v2, float &d2)
{
    float angle = atan((y1-y2)/(x1-x2));
    float xcomp1 = cos(d1-angle)*v1;
    float xcomp2 = cos(d2-angle)*v2;
    addPolar(angle,-1*xcomp2,d2,v2,d2,v2);
    addPolar(angle,xcomp1,d2,v2,d2,v2);
    addPolar(angle,-1*xcomp1,d1,v1,d1,v1);
    addPolar(angle,xcomp2,d1,v1,d1,v1);
}

void drawCue (sf::Sprite &poolcue, sf::Vector2i mouseposition, sf::Time &t1, sf::Time &t2, sf::Clock myclock,float &elapse,vector<float> & y,vector<float> & x,vector<float> & v,vector<float> & d,int &turn){
    float angle = atan(-1*(mouseposition.y-y[0])/(mouseposition.x-x[0]));
    float cue_x= cos(angle);
    float cue_y= sin(angle);
    angle*=180/PI*-1;
    if(mouseposition.x < x[0]){
        cue_x*=-1;
        cue_y*=-1;
        }
    else angle-=180;

    poolcue.setPosition(70*cue_x+x[0],70*cue_y*-1+y[0]);
    poolcue.setRotation(angle+8);

    if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){
        t1 = myclock.getElapsedTime();
        elapse = t1.asSeconds()-t2.asSeconds()+1;
        poolcue.setPosition(70*elapse*cue_x+x[0],70*elapse*cue_y*-1+y[0]);
    }else {
        v[0] = elapse*3;
        if(t1.asSeconds()>t2.asSeconds()){
            elapse=0;
            turn+=1;
        }
        t2 = myclock.getElapsedTime();
        d[0] = angle*PI/180;
    }
}

int main()
{
    sf::RenderWindow gamewindow(sf::VideoMode(965,535),"window",sf::Style::Default);
    gamewindow.setFramerateLimit(80);
    sf::Time t1,t2;
    sf::Clock myclock;
    int turn=1;
    float elapse;
    vector<float> x;
    vector<float> y;
    vector<float> v;
    vector<float> d;
    vector<sf::Texture> textures;
    vector<int> bn;
    setup(x,y,v,d,textures,bn);

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        return EXIT_FAILURE;
    }
    sf::Text text("Player 1",font,30);
    sf::Text text2("Player 2",font,30);
    text.move(20,0);
    text2.move(20,0);

    //add textures for sprites
    sf::Texture texture1;
    if (!texture1.loadFromFile( "pooltable.png")) return EXIT_FAILURE;
    sf::Texture texture2;
    if (!texture2.loadFromFile( "poolcue.png")) return EXIT_FAILURE;

    //create sprites
    sf::Sprite poolcue(texture2);
    poolcue.scale(0.45,0.45); poolcue.setRotation(8); poolcue.setOrigin(500,80);
    sf::Sprite pooltable(texture1);
    pooltable.scale(0.61,0.61);

    while(gamewindow.isOpen())
    {
        sf::Event event;
        while(gamewindow.pollEvent(event))
        {
            if(event.type==sf::Event::Closed) gamewindow.close();
            if(event.type==sf::Event::KeyPressed)
            {
                if(event.key.code==sf::Keyboard::Q) gamewindow.close();
                if(event.key.code==sf::Keyboard::R){
                    setup(x,y,v,d,textures,bn);
                    turn =1;
                }
            }
        }
        gamewindow.clear(sf::Color::White);
        sf::Vector2i mouseposition = sf::Mouse::getPosition(gamewindow);
        gamewindow.draw(pooltable);
        if(v[0]==0){
         drawCue(poolcue,mouseposition,t1,t2, myclock,elapse,y,x,v,d,turn);
         gamewindow.draw(poolcue);
        }
        if(turn%2) gamewindow.draw(text);
        else gamewindow.draw(text2);
        gamewindow.setTitle("Pool Game!");
        moveStones(x,y,v,d);
        bool nframe=false;
        while(!nframe)
        {
            nframe=true;
            for(int i=0;i<x.size();i++)
            {
                for(int j=i+1;j<x.size();j++)
                {
                    if(collisionCheck(x[i],y[i],v[i],d[i],x[j],y[j],v[j],d[j]))
                    {
                        collide(x[i],y[i],v[i],d[i],x[j],y[j],v[j],d[j]);
                        nframe =false;
                    }
                }
            }
        }
        for(int i=0;i<x.size();i++)
        {

            sf::CircleShape circle(RADIUS);
            circle.setOrigin(RADIUS,RADIUS);
            if(i==0) circle.setFillColor(sf::Color::White);
            else circle.setTexture(&textures[i]);
            circle.setPosition(x[i],y[i]);
            gamewindow.draw(circle);
            if(x[i]>914||x[i]<59||y[i]>479||y[i]<54) madeball(x,y,v,d,textures,bn,i);
        }

        gamewindow.display();
    }
    return 0;
}
