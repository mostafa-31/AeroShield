import processing.serial.*;

Serial myPort;

int radarRadius = 300; // Radius of the radar
float scanningAngle = 0; // Current scanning angle
PFont font;

// Data variables
float detectedDistance = -1; // Distance of detected object
int detectedAngle = -1;      // Angle of detected object
boolean objectDetected = false; // Flag to indicate if an object is detected

void setup() {
  size(700, 700);
  background(0);
  
  // Set up serial communication
  myPort = new Serial(this, Serial.list()[0], 115200);
  myPort.bufferUntil('\n');

  // Set up font for displaying text
  font = createFont("Arial", 16, true);
  textFont(font);
}

void draw() {
  background(0);
  
  // Move to center
  translate(width / 2, height / 2);

  // Draw radar base
  stroke(100, 255, 100);
  fill(0);
  ellipse(0, 0, radarRadius * 2, radarRadius * 2);

  // Draw circular grid and distance markings
  stroke(50, 255, 50, 100);
  for (int i = 1; i <= 3; i++) {
    float r = radarRadius * i / 4;
    noFill();
    ellipse(0, 0, r * 2, r * 2);
    
    // Add distance scale
    fill(100, 255, 100);
    textAlign(CENTER, CENTER);
    text(i * 25 + " cm", r, -5);
  }

   // Draw angle markings
  for (int angle = 0; angle < 360; angle += 30) {
    float x1 = cos(radians(angle)) * (radarRadius + 20); // Extended outward
    float y1 = sin(radians(angle)) * (radarRadius + 20); // Extended outward
    float x2 = cos(radians(angle)) * radarRadius;
    float y2 = sin(radians(angle)) * radarRadius;

    stroke(100, 255, 100);
    line(x2, y2, x2 + (x1 - x2) * 0.1, y2 + (y1 - y2) * 0.1); // Shorter tick lines

    // Display angle text a bit outside the circle
    fill(100, 255, 100);
    textAlign(CENTER, CENTER);
    text(angle + "°", x1, y1);
  }


  // Draw radar sweep line
  stroke(0, 255, 0);
  strokeWeight(2);
  float x = cos(radians(scanningAngle)) * radarRadius;
  float y = sin(radians(scanningAngle)) * radarRadius;
  line(0, 0, x, y);

  // Display detected object
  if (objectDetected) {
    float objX = cos(radians(detectedAngle)) * map(detectedDistance, 0, 100, 0, radarRadius);
    float objY = sin(radians(detectedAngle)) * map(detectedDistance, 0, 100, 0, radarRadius);

    fill(255, 0, 0);
    noStroke();
    ellipse(objX, objY, 10, 10);
    
    // Display text information
    fill(255);
    textAlign(LEFT);
    text("Object detected:", -width / 2 + 10, -height / 2 + 20);
    text("Angle: " + detectedAngle + "°", -width / 2 + 10, -height / 2 + 40);
    text("Distance: " + nf(detectedDistance, 0, 1) + " cm", -width / 2 + 10, -height / 2 + 60);
  }

  // Update scanning angle
  scanningAngle += 2; // Increment by 2 degrees
  if (scanningAngle > 360) {
    scanningAngle = 0;
  }

  delay(20); // Control radar sweep speed
}

void serialEvent(Serial myPort) {
  String input = myPort.readStringUntil('\n');
  if (input != null) {
    input = trim(input); // Remove extra whitespace
    String[] data = split(input, ',');

    // Parse incoming data
    if (data.length == 3) {
      
      float distance = float(data[1]); // Distance in cm
      int angle = int(data[2]);       // Angle in degrees

      // Update variables for visualization
      detectedDistance = distance;
      detectedAngle = angle;
      objectDetected = true;
    }
  }
}
