using UnityEngine;
using System.Collections;

public class AIStateManager : MonoBehaviour {
	// Debug Variables
	float debugCountTotal = 1f;
	float debugCount = 0f;


	public enum GuardState {
		Patrol,
		Check,
		Look,
		Chase
	}
	public GuardState guardState;

	// Position and Movement variables
	Vector2 currentPosition;
	float movementSpeed;
	float runSpeed;
	Vector2 direction;
	Vector2 moveTarget;
	Vector2 lastSeenPlayerLocation;

	// Patrol Path Variables
	public Vector2[] pathList = new Vector2[4];
	int pathPointCounter;
	float inRangeOfPoint;
	public bool isPathReversable = true;
	bool reversePath = false;

	// Catch player variables
	bool playerInView;
	float sightRange;
	float noticeTime;
	float totalNoticeTime;
	Vector2 playerPosition;

	// Rotation Variables
	float rotationSpeed;
	float totalRotation;

	void Start () {
		Enemy(0.25f, 5f, 0.85f, 1.5f, 50f, pathList);
	}

	void Enemy(float pathPointRange, float sight, float walk, float run, float turn, Vector2[] path) {
		guardState = GuardState.Patrol;
		
		pathPointCounter = 0;
		pathList = path;

		sightRange =  sight;
		inRangeOfPoint = pathPointRange;
		playerInView = false;

		movementSpeed = walk;
		runSpeed = run;
		rotationSpeed = turn; // In Degrees

		totalRotation = 0f;
		direction = new Vector2(Random.Range(0f, 1f),Random.Range(0f, 1f)).normalized;
		currentPosition = path[0];
		
		totalNoticeTime = 0.5f;
		noticeTime = totalNoticeTime;
	}

	void Update () {
		playerPosition = GameObject.FindGameObjectWithTag("Player").transform.position;//Camera.main.ScreenToWorldPoint(Input.mousePosition);

		switch (guardState) {
		case GuardState.Patrol:
			Patrol();
			break;
		case GuardState.Check:
			Check();
			break;
		case GuardState.Look:
			Look();
			break;
		case GuardState.Chase:
			Chase();
			break;
		}

		CheckVision();

		Debug.DrawRay(transform.position, direction, Color.red);
	}
	
	void Patrol() {
		// Calculate Movement Target
		if ((pathList[pathPointCounter] - currentPosition).magnitude <= inRangeOfPoint) {
			if (pathPointCounter >= pathList.Length-1) {
				if (isPathReversable) {
					reversePath = true;
				} else {
					pathPointCounter = -1;
				}
			} else if (pathPointCounter <= 0) {
				reversePath = false;
			}
			
			if (reversePath) {
				pathPointCounter--;
			} else {
				pathPointCounter++;
			}
		}

		moveTarget = pathList[pathPointCounter];
		Move();
	}
	
	void Check() {
		// The location is set in the check vision
		if ((lastSeenPlayerLocation - currentPosition).magnitude <= inRangeOfPoint) {
			guardState = GuardState.Look;
			noticeTime = 0f;
			totalRotation = 0f;
		}

		moveTarget = lastSeenPlayerLocation;
		Move();
	}
	
	void Look() {
		// Rotate around in a circle
		RotateLook();

		if (totalRotation > 360) {
			// Did not see the player return to the patrol
			guardState = GuardState.Patrol;
		}
	}
	
	void Chase() {
		// Run towards the player target
		moveTarget = lastSeenPlayerLocation;
		Run();

		if ((lastSeenPlayerLocation - currentPosition).magnitude <= inRangeOfPoint) {
			guardState = GuardState.Look;
			noticeTime = 0f;
			totalRotation = 0f;
		}
	}

	void CheckVision() {
		Vector2 playerDirection = (playerPosition - currentPosition).normalized;
		// Check Vision Cone
		if (Vector2.Dot (playerDirection, direction) > 0.5f) {
			if ((playerPosition - currentPosition).magnitude <= sightRange) {
				RaycastHit2D hit = Physics2D.Raycast(currentPosition, (playerPosition - currentPosition));
				if (hit.collider.gameObject.tag == "Player") {
					// enemy can see the player!
					noticeTime += Time.deltaTime;
					// TODO Show a question mark over the guards head to show that he noticed something

					if (noticeTime > totalNoticeTime) {
						//Debug.Log ("Guard noticed the player");
						// TODO Show an exclaimation mark over the guards head to show that he noticed something

						switch (guardState) {
						case GuardState.Patrol:
							// TODO Delay and then set the location if correct
							lastSeenPlayerLocation = playerPosition;
							guardState = GuardState.Check;
							noticeTime = 0f;
							break;
						case GuardState.Check:
							break;
						case GuardState.Look:
							lastSeenPlayerLocation = playerPosition;
							guardState = GuardState.Chase;
							// set a chase var and start a countdown to do a check if you still see the player
							break;
						case GuardState.Chase:
							// set the player position
							lastSeenPlayerLocation = playerPosition;
							noticeTime = totalNoticeTime * 0.5f;
							break;
						}

					}
				}
			}
		} else {
			noticeTime -= Time.deltaTime;

			if (noticeTime <= 0f) {
				noticeTime = 0f;
			}
		}
	}
	
	void Move() {
		direction = (moveTarget - currentPosition).normalized;
		currentPosition += direction * movementSpeed * Time.deltaTime;
		transform.position = currentPosition;
	}
	
	void Run() {
		if ((moveTarget - currentPosition).magnitude > 0.1f) {
			direction = (moveTarget - currentPosition).normalized;
		}
		currentPosition += direction * runSpeed * Time.deltaTime;
		transform.position = currentPosition;
	}

	void RotateLook() {
		totalRotation += rotationSpeed * Time.deltaTime;
		direction = RotateVector2d(direction, rotationSpeed * Time.deltaTime);
		// check for a full rotation, if complete and no player as target then return to the path
	}

	static Vector2 RotateVector2d(Vector2 dir, float degree) {
		float radian = degree * Mathf.PI/180;
		Vector2 result = new Vector2();
		result.x = Mathf.Cos(radian);
		result.y = Mathf.Sin(radian);

		return new Vector2(result.x*dir.x - result.y*dir.y, result.y*dir.x + result.x*dir.y);
	}
}
