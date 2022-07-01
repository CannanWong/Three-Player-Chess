using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Button : MonoBehaviour
{
    public Tile tile;

    private void OnMouseDown()
    {
        int n = 0;
        foreach (GameObject piece in GameManager.instance.white.pieces)
        {
            if (piece.GetComponent<Piece>().tile == tile)
            {
                GameObject tpiece = piece;
                Debug.Log(tile.x + "" + tile.y);
                GameManager.instance.white.pieces.Remove(piece);
                Destroy(tpiece);
                n++;
            }
            if ( n != 0 )
            {
                break;
            }
        }
        foreach (GameObject piece in GameManager.instance.red.pieces)
        {
            if (piece.GetComponent<Piece>().tile == tile)
            {
                GameObject tpiece = piece;
                Debug.Log(tile.x + "" + tile.y);
                GameManager.instance.red.pieces.Remove(piece);
                Destroy(tpiece);
                n++;
            }
            if (n != 0)
            {
                break;
            }
        }
        foreach (GameObject piece in GameManager.instance.black.pieces)
        {
            if (piece.GetComponent<Piece>().tile == tile)
            {
                GameObject tpiece = piece;
                Debug.Log(tile.x + "" + tile.y);
                GameManager.instance.black.pieces.Remove(piece);
                Destroy(tpiece);
                n++;
            }
            if (n != 0)
            {
                break;
            }
        }
        if (tile.transform.GetComponentInParent<Region>().x != GameManager.instance.gamePiece.GetComponentInParent<Region>().x)
        {
            GameManager.instance.gamePiece.transform.SetParent(GameManager.instance.GetRegion(tile.transform.GetComponentInParent<Region>().x).transform);
        }
        GameManager.instance.gamePiece.GetComponent<Piece>().tile = tile;
        GameManager.instance.gamePiece.transform.position = tile.transform.position;
        GameManager.instance.TurnBoard();
        char[] str = new char[3] { (char)tile.GetComponentInParent<Region>().x, (char)tile.x, (char)tile.y };
        GameManager.instance.logcomm.send_msg(new string(str));
        foreach (GameObject button in GameManager.instance.buttons)
        {
            Destroy(button);
            GameManager.instance.buttons.RemoveAll(s => s == null);
        }
    }
}
