package imageConversion;

import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import javax.imageio.ImageIO;

class Main {
  public static void main(String[] args) {
        try {

            String path = "smile.png";
            BufferedImage image = ImageIO.read(new File(path));
            int w = image.getWidth();
            int h = image.getHeight();
            for (int y = 0; y < h; y++) {
                for (int x = 0; x < w; x++) {
                    Color c = new Color(image.getRGB(x, y));
                    int red = c.getRed();
                    int green = c.getGreen();
                    int blue = c.getBlue();
                    System.out.format("strip.setPixelColor(customMappingFunction(%d, %d), strip.Color(%d, %d, %d));\n", x, y, red, green, blue);
                }
            }
        } catch (Exception ex) {
            System.out.println(ex.getMessage());
        }
    }
}