#include <Arduino.h>
#include <Wire.h>
#include <Keypad.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>

// Password config
String password;
const uint8_t NUM_VALID_PASSWORDS = 1;
const uint8_t MAX_PASSWORD_LENGTH = 4;
// +1 to account for '\0'
const char VALID_PASSWORDS[NUM_VALID_PASSWORDS][MAX_PASSWORD_LENGTH + 1] =
	{
		"0054",
};

// Keypad config
const uint8_t ROWS = 4;
const uint8_t COLS = 4;

char keys[ROWS][COLS] = {
	{'1', '2', '3', 'A'},
	{'4', '5', '6', 'B'},
	{'7', '8', '9', 'C'},
	{'*', '0', '#', 'D'}};

uint8_t rowPins[ROWS] = {9, 8, 7, 6};
uint8_t colPins[COLS] = {5, 4, 3, 2};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Servo config
Servo servo;
const uint8_t SERVO_PIN = 12;
const uint8_t SERVO_LOCK_ANGLE = 0;
const uint8_t SERVO_UNLOCK_ANGLE = 80;

// LCD config
LiquidCrystal_I2C lcd(0x27, 16, 2);

void lockBox();
void unlockBox();
void processKey(char);
void validatePassword();
void promptUser();
bool isValidPassword(String);

void setup()
{
	Serial.begin(115200);

	lcd.init();
	lcd.backlight();

	servo.attach(SERVO_PIN);
	servo.write(SERVO_LOCK_ANGLE);

	promptUser();
}

void loop()
{
	char key = keypad.getKey();

	if (key)
	{
		processKey(key);
	}
}

void lockBox()
{
	if (servo.read() == SERVO_LOCK_ANGLE)
		return;

	for (int i = SERVO_UNLOCK_ANGLE; i >= SERVO_LOCK_ANGLE; i--)
	{
		servo.write(i);
		delay(20);
	}
}

void unlockBox()
{
	if (servo.read() == SERVO_UNLOCK_ANGLE)
		return;

	for (int i = SERVO_LOCK_ANGLE; i <= SERVO_UNLOCK_ANGLE; i++)
	{
		servo.write(i);
		delay(20);
	}
}

void processKey(char key)
{
	switch (key)
	{
	case '#':
	{
		lockBox();
		break;
	}

	case '*':
		promptUser();
		password = "";
		break;

	default:
		password += key;
		lcd.setCursor(0, 1);
		lcd.print(password);

		if (password.length() >= MAX_PASSWORD_LENGTH)
		{
			delay(500);
			validatePassword();
		}
		break;
	}
}

void validatePassword()
{
	lcd.clear();
	lcd.home();

	if (isValidPassword(password))
	{
		lcd.print("Correct pin!");
		lcd.setCursor(0, 1);
		lcd.print("Take your gift");
		unlockBox();
	}
	else
	{
		lcd.print("Wrong pin");
	}

	delay(5000);
	promptUser();

	password = "";
}

void promptUser()
{
	lcd.clear();
	lcd.home();
	lcd.print("Enter the pin:");
}

bool isValidPassword(String password)
{
	for (size_t i = 0; i < NUM_VALID_PASSWORDS; i++)
	{
		if (password == VALID_PASSWORDS[i])
			return true;
	}

	return false;
}