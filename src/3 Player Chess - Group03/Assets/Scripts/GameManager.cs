using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using TMPro;

public class GameManager : MonoBehaviour
{
    //Script Access
    public static GameManager instance;

    //Menus
    public Menu gameMenu;
    public Menu endMenu;
    public Menu roundEndMenu;

    //UI
    public TMP_InputField name1if, name2if, name3if;
    public TextMeshProUGUI text;
    public TextMeshProUGUI button1;
    public TextMeshProUGUI button2;
    public TextMeshProUGUI nameDisplay1, nameDisplay2, nameDisplay3;
    public TextMeshProUGUI point1, point2, point3;
    public string name1, name2, name3;

    //Board
    public Region white, red, black;
    public GameObject board;
    public Region currRegion;
    public bool boardCreated = false;
    public GameObject gamePiece;

    public GameObject button;
    public network.logic_connection logcomm = new network.logic_connection();
    public List<GameObject> buttons;
    public int currentInstruction;

    private void Awake()
    {
        instance = this;
    }

    private void Update()
    {
        if (Input.GetKeyDown(KeyCode.Space) && !boardCreated)
        {
            PlaceBoard();
            boardCreated = true;
        }

        if (Input.GetKeyDown(KeyCode.W))
        {
            TurnBoard();
        }
    }

    public void Quit()
    {
        Application.Quit();
    }

    public void SetNames()
    {
        name1 = name1if.text;
        name2 = name2if.text;
        name3 = name3if.text;

        if (name1.Length == 0 || name2.Length == 0 || name3.Length == 0)
        {
            text.text = "All 3 names must be filled in";
        }
        else
        {
            MenuManager.instance.OpenMenu(gameMenu);
            StartLogComms();
        }
    }

    public void PlaceBoard()
    {
        currRegion = white;
        PlaceAllPieces(white);
        PlaceAllPieces(black);
        PlaceAllPieces(red);
    }

    public void StartLogComms()
    {
        logcomm.start_connection();
        logcomm.send_msg("Ready Signal");
    }

    public Region GetRegion(int x)
    {
        if (x == 2)
        {
            return white;
        }
        else if (x == 3)
        {
            return red;
        }
        else
        {
            return black;
        }
    }

    public void PlacePiece(GameObject piece, GameObject tile, Region region)
    {
        gamePiece = Instantiate(piece, tile.transform.position, currRegion.transform.rotation);
        gamePiece.transform.localScale = new Vector3(1.3f, 1.3f, 1);
        gamePiece.GetComponent<Piece>().tile = tile.GetComponent<Tile>();
        gamePiece.transform.parent = tile.transform.parent;
        region.pieces.Add(gamePiece);
    }

    public void EndMenu()
    {
        nameDisplay1.text = name1;
        nameDisplay2.text = name2;
        nameDisplay3.text = name3;
    }

    public void PlaceAllPieces(Region region)
    {
        for (int i = 0; i < 8; i++)
        {
            PlacePiece(region.pawn ,region.coords[1, i], region);
        }
        PlacePiece(region.rook, region.coords[0, 0], region);
        PlacePiece(region.rook, region.coords[0, 7], region);
        PlacePiece(region.knight, region.coords[0, 1], region);
        PlacePiece(region.knight, region.coords[0, 6], region);
        PlacePiece(region.bishop, region.coords[0, 2], region);
        PlacePiece(region.bishop, region.coords[0, 5], region);
        PlacePiece(region.queen, region.coords[0, 4], region);
        PlacePiece(region.king, region.coords[0, 3], region);
    }

    public void SendMessage()
    {

    }

    public void EnablePieces(Region region)
    {
        foreach (GameObject piece in region.pieces)
        {
            piece.GetComponent<CircleCollider2D>().enabled = true;
        }
    }

    public void DisablePieces(Region region)
    {
        foreach (GameObject piece in region.pieces)
        {
            piece.GetComponent<CircleCollider2D>().enabled = false;
        }
    }

    public void TurnBoard()
    {
        if (currRegion == white)
        {
            currRegion = red;
            EnablePieces(red);
            DisablePieces(white);

        } else if (currRegion == red) 
        {
            currRegion = black;
            EnablePieces(black);
            DisablePieces(red);
        }
        else if (currRegion == black)
        {
            currRegion = white;
            EnablePieces(white);
            DisablePieces(black);
        }

        board.transform.Rotate(0, 0, -120);
        foreach (Transform piece in white.transform)
        {
            if (piece.GetComponent<Piece>() != null)
            {
                piece.transform.Rotate(0, 0, 120);
            }
        }
        foreach (Transform piece in red.transform)
        {
            if (piece.GetComponent<Piece>() != null)
            {
                piece.transform.Rotate(0, 0, 120);
            }
        }
        foreach (Transform piece in black.transform)
        {
            if (piece.GetComponent<Piece>() != null)
            {
                piece.transform.Rotate(0, 0, 120);
            }
        }
    }
}
