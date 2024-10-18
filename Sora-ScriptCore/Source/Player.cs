using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Sora;

namespace Sandbox
{
    public class Player : Entity
    {
        void OnCreate()
        {
            Console.WriteLine($"Player.OnCreate - {ID}");
        }

        void OnUpdate(float ts)
        {
            //Console.WriteLine($"Player.OnUpdate: {ts}");

            float speed = 1.0f;
            Vector3 velocity = Vector3.Zero;

            if (Input.IsKeyPressed(KeyCode.W))
            {
                velocity.Y = 1.0f;
            }
            else if(Input.IsKeyPressed(KeyCode.S))
            {
                velocity.Y = -1.0f;
            }

            if (Input.IsKeyPressed(KeyCode.A))
            {
                velocity.X = -1.0f;
            }
            else if (Input.IsKeyPressed(KeyCode.D))
            {
                velocity.X = 1.0f;
            }

            velocity *= speed;

            Vector3 translation = Translation;
            translation += velocity * ts;
            Translation = translation;
        }
    }
}
