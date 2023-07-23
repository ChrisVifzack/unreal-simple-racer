# Simple Racer with AI made in Unreal Engine 5

A demo project showcasing simplistic vehicle movement and PID-controlled vehicle AI. 

It's meant as a reference on how to use Unreal’s game framework (Game Instance, Game Modes, Data Assets, Save Game, etc.), create a Map Selection menu with Widgets, and how to setup an AI Controller with Behavior Trees.

All code was written by myself. Assets were taken from marketplace.


## Checkpoint Racing

- In the race game mode the player has to drive through the checkpoints as fast as possible.
- Once a full lap is completed the time will be saved and stored on the game instance.
- Try to beat you own local highscore! :)

## AI with PID Control

- AI functionality is encapsuled in the RacingAI plugin.
- Two PID controllers are used to control steering and speed respectively. (PID control works by injecting inputs to minimize the error between actual and desired state. This enables the AI to follow the spline.)
- Once splines are placed on the map, the Spline Selection Service will find them and give them to vehicles to follow.

 
[](https://github.com/ChrisVifzack/unreal-simple-racer/assets/12095036/b96a069a-0654-4027-af20-88d97ad3b66c)
