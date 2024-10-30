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
        private TransformComponent m_Transform;
        private Rigidbody2DComponent m_Rigidbody;

        void OnCreate()
        {
            Console.WriteLine($"Player.OnCreate - {ID}");

            m_Transform = GetComponent<TransformComponent>();
            m_Rigidbody = GetComponent<Rigidbody2DComponent>();

            bool hasTransform = HasComponent<TransformComponent>();
            Console.WriteLine($"{hasTransform}");
        }

        void OnUpdate(float ts)
        {
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

            m_Rigidbody.ApplyLinearImpulse(velocity.XY, true);

            /*
            Vector3 translation = m_Transform.Translation;
            translation += velocity * ts;
            m_Transform.Translation = translation;
            */
        }
    }
}
