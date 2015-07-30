#include <SFML/Graphics.hpp>
#include <string>
#include <cmath>
#include <iostream>
#include <random>

#define PI 3.1415
#define RADIUS 15

using namespace std;

default_random_engine generator;
uniform_real_distribution<float> ran(0.0,1.0);

void setup(vector<float> & x,vector<float> & y,vector<float> & v,vector<float> & d)
{
    x.clear();
    y.clear();
    v.clear();
    d.clear();

    float startx[] = {240.0+20*ran(generator),250.0,235.0,266.0,250.0,218.0,282.0,235.0,266.0,203.0,297.0};
    float starty[] = {450.0,250.0,224.0,224.0,198.0,198.0,198.0,172.0,172.0,172.0,172.0};
    float startv[] = {3.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
    float startd[] = {3*PI/2,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};

    for(int i=0;i<11;i++)
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
        v[i]*= .996;
        if(v[i]<.05) v[i]=0;
        if(x[i]>485||x[i]<15) d[i]=PI-d[i];
        if(y[i]>485||y[i]<15) d[i]=2*PI-d[i];
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

void checkCollision(float & x1,float & y1,float & v1,float & d1,float & x2, float & y2, float & v2, float & d2)
{
    float distance = (x1+cos(d1)*v1-x2+cos(d2)*v2)*(x1+cos(d1)*v1-x2+cos(d2)*v2)+(y1+sin(d1)*v1-y2+sin(d2)*v2)*(y1+sin(d1)*v1-y2+sin(d2)*v2);
    if(distance<900)
    {
        float angle = atan((y1-y2)/(x1-x2));
        float xcomp1 = cos(d1-angle)*v1;
        float xcomp2 = cos(d2-angle)*v2;
        addPolar(angle,-1*xcomp2,d2,v2,d2,v2);
        addPolar(angle,xcomp1,d2,v2,d2,v2);
        addPolar(angle,-1*xcomp1,d1,v1,d1,v1);
        addPolar(angle,xcomp2,d1,v1,d1,v1);
    }
}

int main()
{
    sf::RenderWindow gamewindow(sf::VideoMode(500,500),"window",sf::Style::Default);
    gamewindow.setFramerateLimit(80);
    vector<float> x;
    vector<float> y;
    vector<float> v;
    vector<float> d;
    setup(x,y,v,d);
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
        gamewindow.setTitle("Pool Game!");
        moveStones(x,y,v,d);
        for(int i=0;i<x.size();i++)
        {
            for(int j=i+1;j<x.size();j++)
            {
                if(collisionCheck(x[i],y[i],v[i],d[i],x[j],y[j],v[j],d[j]))
                collide(x[i],y[i],v[i],d[i],x[j],y[j],v[j],d[j]);
            }
            sf::CircleShape circle(15);
            circle.setOrigin(15,15);
            circle.setFillColor(sf::Color::Black);
            circle.setPosition(x[i],y[i]);
            gamewindow.draw(circle);
        }
        gamewindow.display();
    }
    return 0;
}
