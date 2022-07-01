using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Piece : MonoBehaviour
{
    public Tile tile;

    private void OnMouseDown()
    {
        foreach (GameObject button in GameManager.instance.buttons)
        {
            Destroy(button);
            GameManager.instance.buttons.RemoveAll(s => s == null);
        }
        char[] str = new char[3] { (char)gameObject.GetComponentInParent<Region>().x, (char)tile.x, (char)tile.y };
        Debug.Log((int)str[0] + ", " + (int)str[1] + ", " + (int)str[2]);
        GameManager.instance.logcomm.send_msg(new string(str));
        string sd = "";
        GameManager.instance.logcomm.receive_msg(ref sd, 192);
        while (sd[0] == 0) {
            GameManager.instance.logcomm.receive_msg(ref sd, 192);
        }
        if (sd[0] != 7)
        {
            for (int i = 0; i < sd.Length; i += 3)
            {
                if (sd[i] == 5)
                {
                    break;
                }
                Debug.Log((int)sd[i]);
                Debug.Log((int)sd[i + 1]);
                Debug.Log((int)sd[i + 2]);
                GameObject button = Instantiate(GameManager.instance.button, GameManager.instance.GetRegion(sd[i]).GetComponent<Region>().coords[sd[i + 2], sd[i + 1]].transform);
                button.transform.parent = gameObject.transform.parent.parent;
                GameManager.instance.buttons.Add(button);
                button.GetComponent<Button>().tile = GameManager.instance.GetRegion(sd[i]).GetComponent<Region>().coords[sd[i + 2], sd[i + 1]].GetComponent<Tile>();
                GameManager.instance.gamePiece = gameObject;
            }
        }
    }
}