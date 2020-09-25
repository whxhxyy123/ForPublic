using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[ExecuteAlways]
public class OcarinaMatCtrl : MonoBehaviour
{
    [Serializable]
    public class MatInfo
    {
        public string desc;
        public MeshRenderer renderer;
        public Texture2D baseMap;
        public float smoothness;
    }

    [SerializeField] public MatInfo[] infos;
    [NonSerialized] private MaterialPropertyBlock block; // 临时变量

    private void Update()
    {
        if(block == null)
        {
            block = new MaterialPropertyBlock();
        }

        Debug.Log("update");
        foreach (MatInfo info in infos)
        {
            if (info.renderer != null)
            {
                block.Clear(); // 清空Block
                if (info.baseMap != null)
                {
                    block.SetTexture("_BaseMap", info.baseMap); // 设置反照率贴图
                }
                block.SetFloat("_Smoothness", info.smoothness); // 设置光滑度
                info.renderer.SetPropertyBlock(block); // 设置Block
            }
        }
    }
}
