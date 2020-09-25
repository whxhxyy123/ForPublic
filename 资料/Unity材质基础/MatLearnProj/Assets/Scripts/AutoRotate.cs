using UnityEngine;
using System.Collections;

public class AutoRotate : MonoBehaviour
{
    public Vector3 rotation;
    public float speed = 10;

    void Update()
    {
        this.transform.Rotate(rotation * Time.deltaTime * speed, Space.World);
    }
}