namespace Util {
  namespace Screen {
    namespace {
      const int pwmFreq = 5000;
      const int pwmResolution = 8;
      const int pwmLedChannelTFT = 0;

      byte orientation = 0;
      byte brightness = 255;

      bool screenOn = true;
    }

    void setRotation(TFT_eSPI& tft, uint8_t rotation) {
      orientation = rotation;
      tft.setRotation(rotation);
    }
    byte getRotation() {
      return orientation;
    }

    // TODO: Probably need to make this configurable, as there may be devices that are the opposite
    bool isPortrait() {
      return orientation == 0 || orientation == 2;
    }
    bool isLandscape() {
      return orientation == 1 || orientation == 3;
    }

    byte getHeight() {
      return orientation % 2 ? TFT_WIDTH : TFT_HEIGHT;
    }

    byte getWidth() {
      return orientation % 2 ? TFT_HEIGHT: TFT_WIDTH;
    }

    void setBrightness(byte b) {
      brightness = b;
      ledcWrite(pwmLedChannelTFT, b);
    }

    void setScreenOn() {
      screenOn = true;
      ledcWrite(pwmLedChannelTFT, brightness);
    }
    void setScreenOff() {
      screenOn = false;
      ledcWrite(pwmLedChannelTFT, 0);
    }

    void fillScreen(TFT_eSPI& tft, uint32_t colour) {
      tft.fillRect(0, 10, getWidth(), getHeight() - 10, colour);
    }

    namespace Grid {
      // x/y = orientation
      // 1/2/3/4 = how many blocks of the screen make up this grid
      // l/t / m / r/b = coordinate for left/top / middle / right/bottom
      // 1 .. blocks = which number block in grid
      // E.g. y3m2() will give you the y coordinate for the middle of the second block in a grid of three
      // E.g. x2r1() will give you the y coordinate for the right of the first block in a grid of two

      int calculate(int len, int pieces, char alignment, int pieceNum) {
        int block = (len / pieces);
        if(alignment == 'l' || alignment == 't') return block * (pieceNum - 1);
        if(alignment == 'm') return block * (pieceNum - 1) + block / 2;
        if(alignment == 'r' || alignment == 'b') return block * pieceNum;
      }

      int x1l1() { return calculate(getWidth(), 1, 'l', 1); }
      int x1m1() { return calculate(getWidth(), 1, 'm', 1); }
      int x1r1() { return calculate(getWidth(), 1, 'r', 1); }

      int x2l1() { return calculate(getWidth(), 2, 'l', 1); }
      int x2m1() { return calculate(getWidth(), 2, 'm', 1); }
      int x2r1() { return calculate(getWidth(), 2, 'r', 1); }
      int x2l2() { return calculate(getWidth(), 2, 'l', 2); }
      int x2m2() { return calculate(getWidth(), 2, 'm', 2); }
      int x2r2() { return calculate(getWidth(), 2, 'r', 2); }

      int x3l1() { return calculate(getWidth(), 3, 'l', 1); }
      int x3m1() { return calculate(getWidth(), 3, 'm', 1); }
      int x3r1() { return calculate(getWidth(), 3, 'r', 1); }
      int x3l2() { return calculate(getWidth(), 3, 'l', 2); }
      int x3m2() { return calculate(getWidth(), 3, 'm', 2); }
      int x3r2() { return calculate(getWidth(), 3, 'r', 2); }
      int x3l3() { return calculate(getWidth(), 3, 'l', 3); }
      int x3m3() { return calculate(getWidth(), 3, 'm', 3); }
      int x3r3() { return calculate(getWidth(), 3, 'r', 3); }

      int x4l1() { return calculate(getWidth(), 4, 'l', 1); }
      int x4m1() { return calculate(getWidth(), 4, 'm', 1); }
      int x4r1() { return calculate(getWidth(), 4, 'r', 1); }
      int x4l2() { return calculate(getWidth(), 4, 'l', 2); }
      int x4m2() { return calculate(getWidth(), 4, 'm', 2); }
      int x4r2() { return calculate(getWidth(), 4, 'r', 2); }
      int x4l3() { return calculate(getWidth(), 4, 'l', 3); }
      int x4m3() { return calculate(getWidth(), 4, 'm', 3); }
      int x4r3() { return calculate(getWidth(), 4, 'r', 3); }
      int x4l4() { return calculate(getWidth(), 4, 'l', 4); }
      int x4m4() { return calculate(getWidth(), 4, 'm', 4); }
      int x4r4() { return calculate(getWidth(), 4, 'r', 4); }

      int y1t1() { return calculate(getHeight() - 10, 1, 't', 1) + 10; }
      int y1m1() { return calculate(getHeight() - 10, 1, 'm', 1) + 10; }
      int y1b1() { return calculate(getHeight() - 10, 1, 'b', 1) + 10; }

      int y2t1() { return calculate(getHeight() - 10, 2, 't', 1) + 10; }
      int y2m1() { return calculate(getHeight() - 10, 2, 'm', 1) + 10; }
      int y2b1() { return calculate(getHeight() - 10, 2, 'b', 1) + 10; }
      int y2t2() { return calculate(getHeight() - 10, 2, 't', 2) + 10; }
      int y2m2() { return calculate(getHeight() - 10, 2, 'm', 2) + 10; }
      int y2b2() { return calculate(getHeight() - 10, 2, 'b', 2) + 10; }

      int y3t1() { return calculate(getHeight() - 10, 3, 't', 1) + 10; }
      int y3m1() { return calculate(getHeight() - 10, 3, 'm', 1) + 10; }
      int y3b1() { return calculate(getHeight() - 10, 3, 'b', 1) + 10; }
      int y3t2() { return calculate(getHeight() - 10, 3, 't', 2) + 10; }
      int y3m2() { return calculate(getHeight() - 10, 3, 'm', 2) + 10; }
      int y3b2() { return calculate(getHeight() - 10, 3, 'b', 2) + 10; }
      int y3t3() { return calculate(getHeight() - 10, 3, 't', 3) + 10; }
      int y3m3() { return calculate(getHeight() - 10, 3, 'm', 3) + 10; }
      int y3b3() { return calculate(getHeight() - 10, 3, 'b', 3) + 10; }

      int y4t1() { return calculate(getHeight() - 10, 4, 't', 1) + 10; }
      int y4m1() { return calculate(getHeight() - 10, 4, 'm', 1) + 10; }
      int y4b1() { return calculate(getHeight() - 10, 4, 'b', 1) + 10; }
      int y4t2() { return calculate(getHeight() - 10, 4, 't', 2) + 10; }
      int y4m2() { return calculate(getHeight() - 10, 4, 'm', 2) + 10; }
      int y4b2() { return calculate(getHeight() - 10, 4, 'b', 2) + 10; }
      int y4t3() { return calculate(getHeight() - 10, 4, 't', 3) + 10; }
      int y4m3() { return calculate(getHeight() - 10, 4, 'm', 3) + 10; }
      int y4b3() { return calculate(getHeight() - 10, 4, 'b', 3) + 10; }
      int y4t4() { return calculate(getHeight() - 10, 4, 't', 4) + 10; }
      int y4m4() { return calculate(getHeight() - 10, 4, 'm', 4) + 10; }
      int y4b4() { return calculate(getHeight() - 10, 4, 'b', 4) + 10; }
    }
  }

  //! Long time delay, it is recommended to use shallow sleep, which can effectively reduce the current consumption
  void espDelay(int ms)
  {
    esp_sleep_enable_timer_wakeup(ms * 1000);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    esp_light_sleep_start();
  }

  namespace Base32 {
    /*
      Base32.cpp - Library for encoding to/decoding from Base32.
      Compatible with RFC 4648 ( http://tools.ietf.org/html/rfc4648 )
      Created by Vladimir Tarasow, December 18, 2012.
      Released into the public domain.
    */
    int toBase32(byte* in, long length, byte*& out, boolean usePadding)
    {
      char base32StandardAlphabet[] = {"ABCDEFGHIJKLMNOPQRSTUVWXYZ234567"};
      char standardPaddingChar = '=';

      int result = 0;
      int count = 0;
      int bufSize = 8;
      int index = 0;
      int size = 0; // size of temporary array
      byte* temp = NULL;

      if (length < 0 || length > 268435456LL)
      {
        return 0;
      }

      size = 8 * ceil(length / 4.0); // Calculating size of temporary array. Not very precise.
      temp = (byte*)malloc(size); // Allocating temporary array.

      if (length > 0)
      {
        int buffer = in[0];
        int next = 1;
        int bitsLeft = 8;

        while (count < bufSize && (bitsLeft > 0 || next < length))
        {
          if (bitsLeft < 5)
          {
            if (next < length)
            {
              buffer <<= 8;
              buffer |= in[next] & 0xFF;
              next++;
              bitsLeft += 8;
            }
            else
            {
              int pad = 5 - bitsLeft;
              buffer <<= pad;
              bitsLeft += pad;
            }
          }
          index = 0x1F & (buffer >> (bitsLeft -5));

          bitsLeft -= 5;
          temp[result] = (byte)base32StandardAlphabet[index];
          result++;
        }
      }

      if (usePadding)
      {
        int pads = (result % 8);
        if (pads > 0)
        {
          pads = (8 - pads);
          for (int i = 0; i < pads; i++)
          {
            temp[result] = standardPaddingChar;
            result++;
          }
        }
      }

      out = (byte*)malloc(result);
      memcpy(out, temp, result);
      free(temp);

      return result;
    }

    int toBase32(byte* in, long length, byte*& out)
    {
      return toBase32(in, length, out, false);
    }

    int fromBase32(byte* in, long length, byte*& out)
    {
      int result = 0; // Length of the array of decoded values.
      int buffer = 0;
      int bitsLeft = 0;
      byte* temp = NULL;

      temp = (byte*)malloc(length); // Allocating temporary array.

      for (int i = 0; i < length; i++)
      {
        byte ch = in[i];

        // ignoring some characters: ' ', '\t', '\r', '\n', '='
        if (ch == 0xA0 || ch == 0x09 || ch == 0x0A || ch == 0x0D || ch == 0x3D) continue;

        // recovering mistyped: '0' -> 'O', '1' -> 'L', '8' -> 'B'
        if (ch == 0x30) { ch = 0x4F; } else if (ch == 0x31) { ch = 0x4C; } else if (ch == 0x38) { ch = 0x42; }


        // look up one base32 symbols: from 'A' to 'Z' or from 'a' to 'z' or from '2' to '7'
        if ((ch >= 0x41 && ch <= 0x5A) || (ch >= 0x61 && ch <= 0x7A)) { ch = ((ch & 0x1F) - 1); }
        else if (ch >= 0x32 && ch <= 0x37) { ch -= (0x32 - 26); }
        else { free(temp); return 0; }

        buffer <<= 5;
        buffer |= ch;
        bitsLeft += 5;
        if (bitsLeft >= 8)
        {
          temp[result] = (unsigned char)((unsigned int)(buffer >> (bitsLeft - 8)) & 0xFF);
          result++;
          bitsLeft -= 8;
        }
      }

      out = (byte*)malloc(result);
      memcpy(out, temp, result);
      free(temp);

      return result;
    }
  }
}
