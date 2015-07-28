#include <SFML/Graphics.hpp>
#include <string>
#include <cmath>
#include <iostream>
#include <random>

#define PI 3.1415

using namespace std;

default_random_engine generator;

void randomize(vector<float> & param,int length,float high,float low,float spacing)
{
    uniform_real_distribution<float> dist(low,high);
    while(param.size()<length)
    {
        double number = dist(generator);
        bool valid = true;
        for(auto a:param) if(abs(a-number)<spacing) valid=false;
        if(valid) param.push_back(number);
    }
}
void setMode(string mode,vector<float> & x,vector<float> & y,vector<float> & v,vector<float> & d,vector<int> & c)
{
    x.clear();
    y.clear();
    v.clear();
    d.clear();
    c.clear();
    if(mode=="random"||mode=="bounce")
    {
        randomize(x,10,485,15,15);
        randomize(y,10,485,15,15);
        randomize(v,10,1,.2,0);
        randomize(d,10,2*PI,0,0);
        for(int i=0;i<20;i++) c.push_back(0);
    }
    if(mode=="test")
    {
        uniform_real_distribution<float> dist(0.0,1.0);

        x.push_back(250.0);
        y.push_back(250.0);
        v.push_back(0.01);
        d.push_back(1.0);

        x.push_back(dist(generator)*30 + 235);
        y.push_back(400);
        v.push_back(1.0);
        d.push_back(3*PI/2);
        c.push_back(0);
        c.push_back(0);
    }
}
void moveStones(vector<float> & x,vector<float> & y,vector<float> & v,vector<float> & d,string mode)
{
    for(int i=0;i<x.size();i++)
    {
        x[i]+= cos(d[i])*v[i];
        y[i]+= sin(d[i])*v[i];
        if(mode=="bounce")
        {
            if(x[i]>485||x[i]<15) d[i]=PI-d[i];
            if(y[i]>485||y[i]<15) d[i]=2*PI-d[i];
        }
        if(mode=="random")
        {
            if(x[i]>515||x[i]<-15||y[i]>515||y[i]<-15)
            {
                uniform_real_distribution<float> dist(0.0,1.0);
                x.erase(x.begin()+i);
                y.erase(y.begin()+i);
                d.erase(d.begin()+i);
                v.erase(v.begin()+i);
                randomize(x,10,485,15,15);
                randomize(y,10,485,15,15);
                randomize(v,10,1,.2,0);
                randomize(d,10,2*PI,0,0);
            }
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
    if(xf<0) tf = PI+atan(yf/xf);
    else tf = atan(yf/xf);
    rf = sqrt(xf*xf+yf*yf);
}

void checkCollision(float & x1, float & y1, float & v1, float & d1, int & c1, float & x2, float & y2, float & v2, float & d2, int & c2)
{
    float distance = sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
    int delay = 10;
    if(distance<30&&c1>delay&&c2>delay)
    {
        float angle = atan((y1-y2)/(x1-x2));
        float xcomp1 = cos(d1-angle)*v1;
        float xcomp2 = cos(d2-angle)*v2;
        addPolar(angle,-1*xcomp2,d2,v2,d2,v2);
        addPolar(angle,xcomp1,d2,v2,d2,v2);
        addPolar(angle,-1*xcomp1,d1,v1,d1,v1);
        addPolar(angle,xcomp2,d1,v1,d1,v1);

        c1=0;
        c2=0;
    }
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(500,500),"window",sf::Style::Default);
    window.setFramerateLimit(80);
    string mode = "bounce";
    vector<float> x;
    vector<float> y;
    vector<float> v;
    vector<float> d;
    vector<int> c;
    setMode(mode,x,y,v,d,c);
    while(window.isOpen())
    {
        sf::Event event;
        while(window.pollEvent(event))
        {
            if(event.type==sf::Event::Closed) window.close();
            if(event.type==sf::Event::KeyPressed)
            {
                if(event.key.code==sf::Keyboard::Q) window.close();
                if(event.key.code==sf::Keyboard::R)
                {
                    mode = "random";
                    setMode(mode,x,y,v,d,c);
                }
                if(event.key.code==sf::Keyboard::B)
                {
                    mode = "bounce";
                    setMode(mode,x,y,v,d,c);
                }
                if(event.key.code==sf::Keyboard::T)
                {
                    mode = "test";
                    setMode(mode,x,y,v,d,c);
                }
            }

        }
        window.clear(sf::Color::White);
        window.setTitle(mode);
        moveStones(x,y,v,d,mode);
        for(int i=0;i<x.size();i++)
        {
            for(int j=i+1;j<x.size();j++) checkCollision(x[i],y[i],v[i],d[i],c[i],x[j],y[j],v[j],d[j],c[j]);
            c[i]++;
            sf::CircleShape circle(15);
            sf::RectangleShape line(sf::Vector2f(15,1));
            line.setPosition(x[i],y[i]);
            line.setRotation(360*d[i]/(2*PI));
            line.setFillColor(sf::Color::Blue);
            circle.setOrigin(15,15);
            circle.setFillColor(sf::Color::Black);
            circle.setPosition(x[i],y[i]);
            window.draw(circle);
            window.draw(line);
        }
        window.display();
    }
    return 0;
}
