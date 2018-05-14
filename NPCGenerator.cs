using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class NPCGenerator : MonoBehaviour
{
    #region Var
    [Range(0, 1000)] public int
        npcToCreate = 1,
        minTargets = 1;
    public GameObject[]
        maleBodies,
        maleFaces,
        maleLegs,
        femaleBodies,
        femaleFaces,
        femaleLegs,
        reptileBodies,
        reptileFaces,
        reptileLegs,
        robotBodies,
        robotFaces,
        robotLegs,
        heads;
    public GameObject furry, ghostWoodleg, wheels;   
    private GameObject[][]
        maleDetails,
        femaleDetails,
        reptileDetails,
        furryDetails,
        robotDetails;
    private GameObject[][][] byGenderDetails;
    private GameManager gameManager;
    private int created;
    private const float bodyOffset = 0.4f;
    private string[]
        maleNames = { "Christ", "Bob", "Martin", "Aymeric", "Valentin", "Nick", "VonBruggel", "Apooh", "Tokigawa", "David" },
        femaleNames = { "Joseline", "Patrice", "Christina", "Céline", "Lisette", "Monique", "De La Buse", "Alexandra", "Siuh", "Granny" },
        reptileNames = { "Neo Ceres", "Neo Mars", "Neo Proxima", "Neo Haumea", "Neo Eris", "Kalliope Ex", "Venus Ex", "Gaspra Ex", "Pluto Ex", "Jupiter Ex" },
        furryNames = { "Kev1n", "J4cob", "B3n", "Th0mas", "Domin1que", "Jo5é", "2oro", "7hibault", "Gre6ory", "Sunsh1ne" },
        botNames = { "X24", "22ROOT", "TR38", "0101", "KK7", "KLM0", "SQD724", "L53D", "XGEN26", "0XXD9" };
    private string[][] surnames;
    #endregion

    private void Start ()
    {
        gameManager = FindObjectOfType<GameManager>();
        CreateArrays();
        created = 0;
        if (minTargets > npcToCreate)
        {
            minTargets = npcToCreate;
        }
        for (int i = 0; i < npcToCreate; i++)
        {
            CreateNewNPC();
            created++;
        }
    }

    private void CreateArrays()
    {
        maleDetails = new GameObject[][] { maleBodies, maleFaces, maleLegs};
        femaleDetails = new GameObject[][] { femaleBodies, femaleFaces, femaleLegs };
        reptileDetails = new GameObject[][] { reptileBodies, reptileFaces, reptileLegs };
        furryDetails = maleDetails;
        robotDetails = new GameObject[][] { robotBodies, robotFaces, robotLegs };
        byGenderDetails = new GameObject[][][] { maleDetails, femaleDetails, reptileDetails, furryDetails, robotDetails};
        surnames = new string[][] { maleNames, femaleNames, reptileNames, furryNames, botNames };
    }

    public void CreateNewNPC()
    {
        GameObject npcObject = new GameObject();
        npcObject.AddComponent<NPC>();
        NPC npc = npcObject.GetComponent<NPC>();
        if (created < minTargets)
        {
            while (npc.isTarget != true)
            {
                SetDetails(npc, true);
                if (CheckDetails(npc) && !CheckExcepts(npc))
                {
                    npc.isTarget = true;
                }
                else
                {
                    npc.isTarget = false;
                }
            }
        }
        else
        {
            SetDetails(npc, false);
            if (CheckDetails(npc) && !CheckExcepts(npc))
            {
                npc.isTarget = true;
            }
            else
            {
                npc.isTarget = false;
            }
        }
        DrawBody(npcObject, npc);
        npc.Init();
    }

    private void SetDetails(NPC npc, bool needTarget)
    {
        npc.gender = (Gender)Random.Range(0, gameManager.gendersLength);
        npc.head = (Head)Random.Range(0, gameManager.headsLength);
        npc.face = (Face)Random.Range(0, gameManager.facesLength);
        npc.body = (Body)Random.Range(0, gameManager.bodiesLength);
        npc.legs = (Legs)Random.Range(0, gameManager.legsLength);
        if (needTarget)
        {
            foreach (string detail in gameManager.targetDetails)
            {
                switch (detail)
                {
                    case "Gender":
                        npc.gender = gameManager.targetGender;
                        break;
                    case "Head":
                        npc.head = gameManager.targetHead;
                        break;
                    case "Face":
                        npc.face = gameManager.targetFace;
                        break;
                    case "Body":
                        npc.body = gameManager.targetBody;
                        break;
                    case "Legs":
                        npc.legs = gameManager.targetLegs;
                        break;
                    default:
                        break;
                }
            }
        }
        if (npc.body == Body.Ghost)
        {
            npc.surname = "Dead ";
        }
        npc.surname = surnames[(int)npc.gender][Random.Range(0, surnames[(int)npc.gender].Length)];
    }

    private bool CheckDetails(NPC npc)
    {
        bool doesMatch = true;
        bool[] matchedDetails = { true, true, true, true, true };
        foreach (string detail in gameManager.targetDetails)
        {
            switch (detail)
            {
                case "Gender":
                    if (npc.gender == gameManager.targetGender)
                    {
                        matchedDetails[0] = true;
                    }
                    else
                    {
                        matchedDetails[0] = false;
                    }
                    break;
                case "Head":
                    if (npc.head == gameManager.targetHead)
                    {
                        matchedDetails[1] = true;
                    }
                    else
                    {
                        matchedDetails[1] = false;
                    }
                    break;
                case "Face":
                    if (npc.face == gameManager.targetFace)
                    {
                        matchedDetails[2] = true;
                    }
                    else
                    {
                        matchedDetails[2] = false;
                    }
                    break;
                case "Body":
                    if (npc.body == gameManager.targetBody)
                    {
                        matchedDetails[3] = true;
                    }
                    else
                    {
                        matchedDetails[3] = false;
                    }
                    break;
                case "Legs":
                    if (npc.legs == gameManager.targetLegs)
                    {
                        matchedDetails[4] = true;
                    }
                    else
                    {
                        matchedDetails[4] = false;
                    }
                    break;
                default:
                    break;
            }
        }
        for (int i = 0; i < matchedDetails.Length; i++)
        {
            if (!matchedDetails[i])
            {
                doesMatch = false;
                break;
            }
        }
        return doesMatch;
    }

    private bool CheckExcepts(NPC npc)
    {
        bool doesMatch = true;
        bool[] matchedExcepts = { true, true, true, true, true };
        foreach (string except in gameManager.exceptDetails)
        {
            switch (except)
            {
                case "Gender":
                    if (npc.gender == gameManager.exceptGender)
                    {
                        matchedExcepts[0] = true;
                    }
                    else
                    {
                        matchedExcepts[0] = false;
                    }
                    break;
                case "Head":
                    if (npc.head == gameManager.exceptHead)
                    {
                        matchedExcepts[1] = true;
                    }
                    else
                    {
                        matchedExcepts[1] = false;
                    }
                    break;
                case "Face":
                    if (npc.face == gameManager.exceptFace)
                    {
                        matchedExcepts[2] = true;
                    }
                    else
                    {
                        matchedExcepts[2] = false;
                    }
                    break;
                case "Body":
                    if (npc.body == gameManager.exceptBody)
                    {
                        matchedExcepts[3] = true;
                    }
                    else
                    {
                        matchedExcepts[3] = false;
                    }
                    break;
                case "Legs":
                    if (npc.legs == gameManager.exceptLegs)
                    {
                        matchedExcepts[4] = true;
                    }
                    else
                    {
                        matchedExcepts[4] = false;
                    }
                    break;
                default:
                    break;
            }
        }
        for (int i = 0; i < matchedExcepts.Length; i++)
        {
            if (!matchedExcepts[i])
            {
                doesMatch = false;
                break;
            }
        }
        if (gameManager.exceptDetails.Count > 0)
        {
            return doesMatch;
        }
        else
        {
            return false;
        }
    }

    private void DrawBody(GameObject obj, NPC npc)
    {
        GameObject
            body,
            legs,
            head,
            face;
        body = Instantiate(byGenderDetails[(int)npc.gender][0][(int)npc.body], obj.transform);
        body.name = npc.surname + "_Body";
        if (npc.body == Body.Ghost && npc.legs == Legs.Woodleg)
        {
            legs = Instantiate(ghostWoodleg, obj.transform);
        }
        else if (npc.legs == Legs.Wheels)
        {
            legs = Instantiate(wheels, obj.transform);
        }
        else
        {
            legs = Instantiate(byGenderDetails[(int)npc.gender][2][(int)npc.legs], obj.transform);
        }
        legs.name = npc.surname + "_Legs";
        if (npc.head > 0)
        {
            head = Instantiate(heads[(int)npc.head - 1], body.transform);
            head.name = npc.surname + "_Head";
        }
        if (npc.face > 0)
        {
            face = Instantiate(byGenderDetails[(int)npc.gender][1][(int)npc.face - 1], body.transform);
            face.name = npc.surname + "_Face";
        }
        if (npc.gender == Gender.Furry)
        {
            GameObject furryInstance = Instantiate(furry, body.transform);
            furryInstance.name = npc.surname + "_Furry";
        }
        if (npc.legs == Legs.Wheels)
        {
            body.transform.localPosition = new Vector3(0, -bodyOffset, 0);
        }
    }
}
