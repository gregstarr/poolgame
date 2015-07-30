#include <SFML/Graphics.hpp>
#include <string>
#include <cmath>
#include <iostream>
#include <random>

#define PI 3.1415

using namespace std;

default_random_engine generator;
uniform_real_distribution<float> dist(0.0,1.0);

void setup(vector<float> & x,vector<float> & y,vector<float> & v,vector<float> & d)
{
    x.clear();
    y.clear();
    v.clear();
    d.clear();

    float startx[] = {220.0,540.0,567.0,567.0,593.0,593.0,593.0,620.0,620.0,620.0,620.0,647.0,647.0,647.0,647.0,647.0};
    float starty[] = {210.0+20*dist(generator),217.0,232.0,201.0,248.0,217.0,186.0,263.0,232.0,201.0,170.0,279.0,248.0,217.0,186.0,155.0};
    float startv[] = {10.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
    float startd[] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};

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
        v[i]*= .996;
        if(v[i]<.15) v[i]=0;
        if(x[i]>732||x[i]<60) d[i]=PI-d[i];
        if(y[i]>378||y[i]<55) d[i]=2*PI-d[i];
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
    sf::RenderWindow gamewindow(sf::VideoMode(790,435),"window",sf::Style::Default);
    gamewindow.setFramerateLimit(60);
    vector<float> x;
    vector<float> y;
    vector<float> v;
    vector<float> d;
    setup(x,y,v,d);
    //add textures for sprites
     sf::Texture texture1;
    if (!texture1.loadFromFile( "pooltable.png")) {
        return EXIT_FAILURE;
    }
    sf::Texture texture2;
    if (!texture2.loadFromFile( "poolcue.png")) {
        return EXIT_FAILURE;
    }
    //create sprites
    sf::Sprite poolcue(texture2);
    poolcue.scale(0.45,0.45); poolcue.setRotation(8); poolcue.setOrigin(500,80);
    sf::Sprite pooltable(texture1);
    pooltable.scale(0.5,0.5);

    int balltexture[] = {0,1,3,11,14,8,6,9,4,15,13,12,5,10,2,7};

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
                if(event.key.code==sf::Keyboard::Up) poolcue.rotate(-3);
                if(event.key.code==sf::Keyboard::Down) poolcue.rotate(3);
            }

        }
        gamewindow.clear(sf::Color::White);
        sf::Vector2i mouseposition = sf::Mouse::getPosition(gamewindow);
        poolcue.setPosition(mouseposition.x,mouseposition.y);
        gamewindow.draw(pooltable);
        gamewindow.draw(poolcue);
        gamewindow.setTitle("Pool Game!");
        moveStones(x,y,v,d);
        for(int i=0;i<x.size();i++)
        {
            for(int j=i+1;j<x.size();j++) checkCollision(x[i],y[i],v[i],d[i],x[j],y[j],v[j],d[j]);
            sf::CircleShape circle(15);
            circle.setOrigin(15,15);
            sf::Texture texture;
            if (!texture.loadFromFile( std::to_string(balltexture[i])+".png")) {
                return EXIT_FAILURE;
            }

            if(i==0) circle.setFillColor(sf::Color::White);
            else circle.setTexture(&texture);
            circle.setPosition(x[i],y[i]);
            gamewindow.draw(circle);
        }
        gamewindow.display();
    }
    return 0;
}
