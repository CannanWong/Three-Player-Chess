using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Region : MonoBehaviour
{
    public GameObject[,] coords = new GameObject[4,8];
    public int x;
    public GameObject pawn;
    public GameObject knight;
    public GameObject bishop;
    public GameObject rook;
    public GameObject queen;
    public GameObject king;
    public List<GameObject> pieces = new List<GameObject>();

    private void Start()
    {
        int a = 0;
        foreach (Transform coord in transform)
        {
            coords[a/8, a%8] = coord.gameObject;
            coord.gameObject.GetComponent<Tile>().x = a % 8;
            coord.gameObject.GetComponent<Tile>().y = a / 8;
            a++;
        }
    }
}

