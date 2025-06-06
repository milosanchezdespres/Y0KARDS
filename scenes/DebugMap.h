struct DebugMap : public Scene
{
    Color textcolor;
    Color textcolor2;

    int i;
    float direction;

    std::string fpsstr;

    Texture2D* player_walk = nullptr;
    Texture2D* player_walk2 = nullptr;

    DebugMap() : Scene() {}

    void __on__init__() override
    {
        gload(player_walk, "player/playerwalk.bmp");
        gload(player_walk2, "player/playerwalk.bmp");

        i = 0;
        direction = 1;

        textcolor = to_color("#ffffff");
        textcolor2 = to_color("#632708");
    }

    void __on__update__() override
    {
        i += direction * 250 * delta;

        if(player_walk)
        {
            if (i > SCREEN_WIDTH - player_walk->width) 
            {
                i = SCREEN_WIDTH - player_walk->width;
                direction = -1;
            }
            else if (i < 0)
            {
                i = 0;
                direction = 1;
            }
        }

        fpsstr = "FPS :" + std::to_string(fps);
    }

    void __on__draw__() override
    {
        if(player_walk) blit(player_walk, S(16, 0, 16, 32, 3, 3), i, 200);
        if(player_walk2) blit(player_walk2, 40, 270, 2);

        TEXT(fpsstr.c_str(), 20, 20, textcolor, 2);
        TEXTBOX("Some text displayed >>on screen..", T(0, 100, SCREEN_WIDTH, SCREEN_HEIGHT), textcolor2, 2);
        RTBOX("Some text displayed >>on screen..", T(0, 200, SCREEN_WIDTH, SCREEN_HEIGHT), 2);
    }
};
