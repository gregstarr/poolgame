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
    float dsq = (x1-x2)*(x1-x2)+(y1-y2)*(y1-y2);
    if(dsq<((2*RADIUS)*(2*RADIUS))) return false;
    float V[] = {v1*cos(d1)-v2*cos(d2),v1*sin(d1)-v2*sin(d2)};
    float C[] = {x2-x1,y2-y1};
    float dot = V[0]*C[0]+V[1]*C[1];
    if(dot<=0) return false;
    float mag = sqrt(V[0]*V[0]+V[1]*V[1]);
    float N[] = {V[0]/mag,V[1]/mag};
    float D = N[0]*C[0]+N[1]*C[1];
    float F = C[0]*C[0]+C[1]*C[1]-D*D;
    if(F>(2*RADIUS)*(2*RADIUS)) return false;
    float T = (2*RADIUS)*(2*RADIUS)-F;
    float dtm = D-sqrt(T);
    if(dtm>mag) return false;
    return true;
}


void setup(vector<float> & x,vector<float> & y,vector<float> & v,vector<float> & d)
{
    x.clear();
    y.clear();
    v.clear();
    d.clear();

    float startx[] = {220.0,700.0,723.0,723.0,746.0,746.0,746.0,769.0,769.0,769.0,769.0,792.0,792.0,792.0,792.0,792.0};
    float starty[] = {247.0+40*dist(generator),267.0,281.0,253.0,295.0,267.0,239.0,309.0,281.0,253.0,225.0,323.0,295.0,267.0,239.0,211.0};
    float startv[] = {15.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
    float startd[] = {-PI/10+PI*dist(generator)/5,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};

    for(int i=0;i<16;i++)
    {
        x.push_back(startx[i]);
        y.push_back(starty[i]);
        v.push_back(startv[i]);
        d.push_back(startd[i]);
    }
}

void moveStones(vector<float> & x,vector<float> & y,vector<float> & v,vector<float> & d)
{
    for(int i=0;i<x.size();i++)
    {
        x[i]+= cos(d[i])*v[i];
        y[i]+= sin(d[i])*v[i];
        if(v[i]>0) v[i]-=.04;
        if(v[i]<.15) v[i]=0;
        if(x[i]+RADIUS>912||x[i]-RADIUS<52) d[i]=PI-d[i];
        if(y[i]+RADIUS>482||y[i]-RADIUS<55) d[i]=2*PI-d[i];
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

int main()
{
    sf::RenderWindow gamewindow(sf::VideoMode(965,535),"window",sf::Style::Default);
    gamewindow.setFramerateLimit(80);
    sf::Time t1,t2;
    sf::Clock myclock;

    vector<float> x;
    vector<float> y;
    vector<float> v;
    vector<float> d;
    setup(x,y,v,d);

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

    int balltexture[] = {0,1,3,11,14,8,6,9,4,15,13,12,5,10,2,7};
    vector<sf::Texture> textures;
    for(auto ball:balltexture)
    {
        sf::Texture texture;
        texture.loadFromFile(to_string(ball)+".png");
        textures.push_back(texture);
    }

    while(gamewindow.isOpen())
    {
        sf::Event event;
        while(gamewindow.pollEvent(event))
        {
            if(event.type==sf::Event::Closed) gamewindow.close();
            if(event.type==sf::Event::KeyPressed)
            {
                if(event.key.code==sf::Keyboard::Q) gamewindow.close();
                if(event.key.code==sf::Keyboard::R) setup(x,y,v,d);
            }
        }
        gamewindow.clear(sf::Color::White);
        sf::Vector2i mouseposition = sf::Mouse::getPosition(gamewindow);
        gamewindow.draw(pooltable);
        float elapse;
        if(v[0]==0)
        {
            float angle = atan(-1*(mouseposition.y-y[0])/(mouseposition.x-x[0]));
            float angle2 = angle;
            float cue_x= cos(angle);
            float cue_y= sin(angle);
            angle*=180/PI*-1;
            if(mouseposition.x < x[0])
            {
                cue_x*=-1;
                cue_y*=-1;
            }
            else angle-=180;

            poolcue.setPosition(40*cue_x+x[0],40*cue_y*-1+y[0]);
            poolcue.setRotation(angle+8);

            if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                t1 = myclock.getElapsedTime();
                elapse = t1.asSeconds()-t2.asSeconds()+1;
                poolcue.setPosition(40*elapse*cue_x+x[0],40*elapse*cue_y*-1+y[0]);
            }
            else
            {
                v[0]=elapse*3;
                if(t1.asSeconds()>t2.asSeconds()) elapse =0;
                t2 = myclock.getElapsedTime();
                d[0] = angle*PI/180;
            }
            gamewindow.draw(poolcue);
        }

        gamewindow.setTitle("Pool Game!");
        moveStones(x,y,v,d);
        bool nframe=false;
        vector<int> first;
        vector<int> second;
        while(!nframe)
        {
            first.clear();
            second.clear();
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
        }
        gamewindow.display();
    }
    return 0;
}
