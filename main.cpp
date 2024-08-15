#include <simplecpp>
#include "shooter.h"
#include "bubble.h"

/* Simulation Vars */
const double STEP_TIME = 0.02;

/* Game Vars */
const int PLAY_Y_HEIGHT = 450;
const int LEFT_MARGIN = 70;
const int TOP_MARGIN = 20;
const int BOTTOM_MARGIN = (PLAY_Y_HEIGHT+TOP_MARGIN);


void move_bullets(vector<Bullet> &bullets){
    // move all bullets
    for(unsigned int i=0; i<bullets.size(); i++){
        if(!bullets[i].nextStep(STEP_TIME)){
            bullets.erase(bullets.begin()+i);
        }
    }
}

void move_bubbles(vector<Bubble> &bubbles){
    // move all bubbles
    for (unsigned int i=0; i < bubbles.size(); i++)
    {
        bubbles[i].nextStep(STEP_TIME);
    }
}

bool shooter_bubble_collision(vector<Bubble> &bubbles, Shooter shooter){
    //check for collisions
    for (unsigned int i=0; i < bubbles.size(); i++)
    {
        if(((bubbles[i].get_center_x() <= shooter.get_head_center_x() + bubbles[i].get_radius() + shooter.get_head_radius()) && (bubbles[i].get_center_x() >= shooter.get_head_center_x() - bubbles[i].get_radius() - shooter.get_head_radius())
            &&
            (bubbles[i].get_center_y() <= shooter.get_head_center_y() + bubbles[i].get_radius() + shooter.get_head_radius()) && (bubbles[i].get_center_y() >= shooter.get_head_center_y() - bubbles[i].get_radius() - shooter.get_head_radius()))
            ||
            (((bubbles[i].get_center_x() <= shooter.get_body_center_x() + shooter.get_body_width()/2 + bubbles[i].get_radius()) &&(bubbles[i].get_center_x() >= shooter.get_body_center_x() - shooter.get_body_width()/2 - bubbles[i].get_radius()))
            &&
            ((bubbles[i].get_center_y() <= shooter.get_body_center_y() + shooter.get_body_height()/2 + bubbles[i].get_radius()) &&(bubbles[i].get_center_y() >= shooter.get_body_center_y() - shooter.get_body_height()/2 - bubbles[i].get_radius())))
            )
            {
                cout << "*** GAME OVER ***" << endl;
                return false;
            }
    }
    return true;
}

void split_big_bubble(vector<Bubble> &bubbles, int i, int* level){
    //split large bubble into smaller ones
    int lvl = *level;

    //create two small bubbles
    bubbles.push_back(Bubble(bubbles[i].get_center_x(), bubbles[i].get_center_y(), BUBBLE_DEFAULT_RADIUS, BUBBLE_DEFAULT_VX*lvl, bubbles[i].get_vy(),  COLOR(155,100*lvl,180*lvl)));
    bubbles.push_back(Bubble(bubbles[i].get_center_x(), bubbles[i].get_center_y(), BUBBLE_DEFAULT_RADIUS, -BUBBLE_DEFAULT_VX*lvl, bubbles[i].get_vy(),  COLOR(155,100*lvl,180*lvl)));
    //remove existing large bubble
    bubbles.erase(bubbles.begin()+i);
}

void bullet_bubble_collision(vector<Bubble> &bubbles, vector<Bullet> &bullets, int* level){
    //check for collisions
    for (unsigned int i=0; i < bubbles.size(); i++)
    {
        for(unsigned int j=0; j < bullets.size(); j++){
            if ((bubbles[i].get_center_x() <= bullets[j].get_center_x() + (bullets[j].get_width()/2) + bubbles[i].get_radius())
                &&
                ((bubbles[i].get_center_x() >= bullets[j].get_center_x() - (bullets[j].get_width()/2) - bubbles[i].get_radius()))
                &&
                ((bubbles[i].get_center_y() <= bullets[j].get_center_y() + (bullets[j].get_height()/2) + bubbles[i].get_radius()))
                &&
                ((bubbles[i].get_center_y() >= bullets[j].get_center_y() - (bullets[j].get_height()/2) - bubbles[i].get_radius())))
                {
                    bullets.erase(bullets.begin()+j);

                    //for small bubbles, erase the bubble :
                    if (bubbles[i].get_radius() == BUBBLE_DEFAULT_RADIUS){
                        bubbles.erase(bubbles.begin()+i);
                    }
                    //for large bubbles, split the bubble :
                    else if(bubbles[i].get_radius() == 2*BUBBLE_DEFAULT_RADIUS){
                        split_big_bubble(bubbles, i, level);
                    }
                }
        }
    }
}

vector<Bubble> create_bubbles(int* level){
    // create initial bubbles in the game
    vector<Bubble> bubbles;
    int lvl = *level;
    bubbles.push_back(Bubble(2*WINDOW_X/3.0, BUBBLE_START_Y, 2*BUBBLE_DEFAULT_RADIUS, -BUBBLE_DEFAULT_VX*lvl, 0,  COLOR(155,100*lvl,180*lvl)));
    bubbles.push_back(Bubble(WINDOW_X/3.0, BUBBLE_START_Y, 2*BUBBLE_DEFAULT_RADIUS, BUBBLE_DEFAULT_VX*lvl, 0, COLOR(155,100*lvl,180*lvl)));
    //bubble speed in x depends on level
    return bubbles;
}

void check_win(vector<Bubble> &bubbles, int* level){
    //increases level and created new bubbles if previous level is complete
    if (bubbles.size() == 0){
        *level = *level + 1;
        bubbles = create_bubbles(level);
    }
    return;
}

int main()
{
    initCanvas("Bubble Trouble", WINDOW_X, WINDOW_Y);

    Line b1(0, PLAY_Y_HEIGHT, WINDOW_X, PLAY_Y_HEIGHT);
    b1.setColor(COLOR(0, 0, 255));

    string msg_cmd("Cmd: _");
    Text charPressed(LEFT_MARGIN, BOTTOM_MARGIN, msg_cmd);

    int level = 1;

    // Intialize the shooter
    Shooter shooter(SHOOTER_START_X, SHOOTER_START_Y, SHOOTER_VX);

    // Initialize the bubbles
    vector<Bubble> bubbles = create_bubbles(&level);

    // Initialize the bullets (empty)
    vector<Bullet> bullets;

    XEvent event;

    // Main game loop
    while (shooter_bubble_collision(bubbles, shooter))
    {
        bool pendingEvent = checkEvent(event);
        if (pendingEvent)
        {
            // Get the key pressed
            char c = charFromEvent(event);
            msg_cmd[msg_cmd.length() - 1] = c;
            charPressed.setMessage(msg_cmd);

            // Update the shooter
            if(c == 'a')
                shooter.move(STEP_TIME, true);
            else if(c == 'd')
                shooter.move(STEP_TIME, false);
            else if(c == 'w')
                bullets.push_back(shooter.shoot());
            else if(c == 'q')
                return 0;
        }
        //check bubble bullet collision
        bullet_bubble_collision(bubbles, bullets, &level);

        // Update the bubbles
        move_bubbles(bubbles);

        // Update the bullets
        move_bullets(bullets);

        wait(STEP_TIME);

        //check if level is complete
        check_win(bubbles, &level);

    }

}
