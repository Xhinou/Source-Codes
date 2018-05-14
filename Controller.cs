using System.Collections;
using UnityEngine;

public class Controller : MonoBehaviour
{
    #region Var
    public GameObject bloodParticles;

    public float
        camTranslate = 0.5f,
        camRotation = 0.5f,
        screenMargin = 20f,
        zoomScale = 10f;
    private GameGround gameGround;
    private Camera mainCam;
    private Transform camTransf, groundTransf;

    private bool
        isZooming,
        controlling = true;
    private bool
        moveForward = true,
        moveBackward = true,
        moveLeft = true,
        moveRight = true;
    private int zoomState = 3;

    private int
        screenWidth,
        screenHeight;
    #endregion

    private void Start()
    {
        screenWidth = Screen.width;
        screenHeight = Screen.height;
        Debug.Log("Your screen size is " + screenWidth + "x" + screenHeight);
        gameGround = FindObjectOfType<GameGround>();
        groundTransf = gameGround.groundTransf;
        mainCam = gameObject.GetComponentInChildren<Camera>();
        camTransf = gameObject.transform;
    }

    private void Update()
    {
        if( Input.GetButtonDown( "Screenshot" ) )
        {
            ScreenCapture.CaptureScreenshot( "main.png", 1 );
        }
        if (controlling)
        {
            if (Input.GetButtonDown("Click"))
            {
                RaycastHit hit;
                if (Physics.Raycast(mainCam.ScreenPointToRay(Input.mousePosition), out hit, 1000f))
                {
                    Transform npcTransf = hit.transform;
                    if (npcTransf.tag == "NPC")
                    {
                        NPC hitNPC = npcTransf.gameObject.GetComponent<NPC>();
                        StartCoroutine(Die(hit));
                        if (hitNPC.isTarget)
                        {
                            Debug.Log("Good target !");
                        }
                        else
                        {
                            Debug.Log("Wrong target !");
                        }
                    }
                }
            }
            if (!Input.GetButton("Rotation"))
            {
                if (Input.mousePosition.x >= screenWidth - screenMargin && moveRight)
                {
                    camTransf.Translate(camTranslate, 0, 0);
                }
                else if (Input.mousePosition.x <= screenMargin && moveLeft)
                {
                    camTransf.Translate(-camTranslate, 0, 0);
                }
                if (Input.mousePosition.y >= screenHeight - screenMargin && moveForward)
                {
                    camTransf.Translate(0, 0, camTranslate);
                }
                else if (Input.mousePosition.y <= screenMargin && moveBackward)
                {
                    camTransf.Translate(0, 0, -camTranslate);
                }
            }
            else
            {
                if (Input.mousePosition.x >= screenWidth -screenMargin)
                {
                    camTransf.Rotate(0, camRotation, 0);
                }
                else if (Input.mousePosition.x <= screenMargin)
                {
                    camTransf.Rotate(0, -camRotation, 0);
                }
            }
            if (!isZooming)
            {
                if (Input.GetAxis("Mouse ScrollWheel") > 0)
                {
                    if (zoomState > 0)
                    {
                        zoomState--;
                        StartCoroutine(CameraZoom(true));
                    }
                }
                else if (Input.GetAxis("Mouse ScrollWheel") < 0)
                {
                    if (zoomState < 3)
                    {
                        zoomState++;
                        StartCoroutine(CameraZoom(false));
                    }
                }
            }
        }
    }

    private void OnApplicationFocus(bool focus)
    {
        controlling = focus;
    }

    private IEnumerator CameraZoom(bool zoomIn)
    {
        isZooming = true;
        if (zoomIn)
        {
            float toFoV = mainCam.fieldOfView - zoomScale;
            while (mainCam.fieldOfView > toFoV)
            {
                mainCam.fieldOfView--;
                yield return null;
            }
        }
        else
        {
            float toFoV = mainCam.fieldOfView + zoomScale;
            while (mainCam.fieldOfView < toFoV)
            {
                mainCam.fieldOfView++;
                yield return null;
            }
        }
        isZooming = false;
    }

    private IEnumerator Die(RaycastHit hit)
    {
        Vector3 npcPos = hit.transform.position;
        GameObject npcBlood = Instantiate(bloodParticles, npcPos, Quaternion.identity);
        ParticleSystem npcBloodParticles = npcBlood.GetComponent<ParticleSystem>();
        npcBloodParticles.collision.SetPlane(0, groundTransf);
        Destroy(hit.transform.gameObject);
        Debug.Log("You've killed an NPC !");
        while (npcBloodParticles.IsAlive())
            yield return null;
        Destroy(npcBloodParticles.gameObject);
    }
}
