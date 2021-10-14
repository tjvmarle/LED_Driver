#include <Arduino.h>

// Analog pins on the Nano reading the values from the connected potmeters
constexpr int analog_pin_red{A0};
constexpr int analog_pin_green{A1};
constexpr int analog_pin_blue{A2};
constexpr int random_seed_pin{A3}; // Unconnected

// Nano pins to the MOSFETs' base pins
constexpr int red_pin{3};
constexpr int green_pin{5};
constexpr int blue_pin{6};
constexpr int driver_pins[3] = {red_pin, green_pin, blue_pin};

constexpr int C_COLOR_PAUSE{10000}; //Time in ms before the change to the next color starts

//! @brief Small struct to denominate a color. All possible colors are the result of mixing red, green and blue LEDs
struct Color
{
  int red{};
  int green{};
  int blue{};

  Color(int r, int g, int b) : red{r}, green{g}, blue{b} {};

  bool operator==(const Color &rhs)
  {
    return (red == rhs.red) and (green == rhs.green) and (blue == rhs.blue);
  }
};

//! @brief Fades the colors in a smooth fasion from one color to the next. Also keeps maximum combined intensity below
//         255 (a single color at max brightness) to avoid burning out the power supply
//! @param[in] from_clr    - Starting color
//! @param[in] to_clr      - Resulting color after the fade
//! @param[in] change_time - Required time to change from the starting color to the resulting one
void change_to_color(const Color &from_clr, const Color &to_clr, const double change_time = 45.0)
{
  constexpr int C_STEPS_PER_SECOND{25}; // Max nr of analogWrites per second
  const double total_change_steps{change_time * static_cast<double>(C_STEPS_PER_SECOND)};

  // Color delta per step, can be negative
  const double red_delta{(static_cast<double>(to_clr.red - from_clr.red)) / total_change_steps};
  const double green_delta{(static_cast<double>(to_clr.green - from_clr.green)) / total_change_steps};
  const double blue_delta{(static_cast<double>(to_clr.blue - from_clr.blue)) / total_change_steps};

  constexpr int C_WRITE_DELAY{1000 / C_STEPS_PER_SECOND}; // Could have rounding errors, but that's acceptable

  for (int i{0}; i < total_change_steps; i++)
  {
    analogWrite(red_pin, from_clr.red + red_delta * i);
    analogWrite(green_pin, from_clr.green + green_delta * i);
    analogWrite(blue_pin, from_clr.blue + blue_delta * i);
    delay(C_WRITE_DELAY);
  }

  // Just to ensure no rounding errors
  analogWrite(red_pin, to_clr.red);
  analogWrite(green_pin, to_clr.green);
  analogWrite(blue_pin, to_clr.blue);
  delay(C_WRITE_DELAY);
}

// red, green, blue, cyan, magenta, yellow, white
Color colors[] = {{254, 0, 0}, {0, 254, 0}, {0, 0, 254}, {0, 127, 127}, {127, 0, 127}, {127, 127, 0}, {85, 85, 85}};
constexpr int max_rand{sizeof(colors) / sizeof(*colors)};
Color prev_color{0, 0, 0};

void setup()
{
  for (const auto pin : driver_pins)
  {
    pinMode(pin, OUTPUT);
  }

  randomSeed(analogRead(random_seed_pin));
  random(); //Without this it seems to boot into red every time.

  const auto &next_color{colors[random(0, max_rand)]};
  change_to_color(prev_color, next_color, 3);
  prev_color = next_color;
}

void loop()
{
  auto &next_color{colors[random(0, max_rand)]};
  while (prev_color == next_color)
  {
    next_color = colors[random(0, max_rand)];
  }

  change_to_color(prev_color, next_color);
  prev_color = next_color;

  delay(C_COLOR_PAUSE);
}

// These values are rouhgly the max values that the analog pins will measure. These can be used for normalization purposes. Don't forget that it IS possible for the pins to exceed the Max value noted here.
// Rd Min: 0, Max: 980
// Gr Min: 0, Max: 975
// Bl Min: 0, Max: 975