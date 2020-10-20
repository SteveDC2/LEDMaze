#!/usr/bin/perl -w
#use strict;
$| = 1;

############################################################################################################
############################################################################################################
##                                                                                                        ##
## Visit http://www.ezimba.com/index-ln.html and select 'reduce color' to generate a reduced color image  ##
##                                                                                                        ##
############################################################################################################
############################################################################################################

#Resultant output data...
#uint16_t : Width
#uint16_t  : Height
#uint8_t  : Type
#uint8_t  : Palette entries
#uint16_t  : ImageWords
#uint16_t : Color[0]
#uint16_t : Color[1]
#...
#uint16_t : Color[n]n
#uint16_t : ImageData[0]
#uint16_t : ImageData[1]
#...
#uint16_t : ImageData[n]

#Read a 24 bit BMP and RLE compress to paletized image
#16 bit entries in data table
#Type 0 = uncompressed : 0  bits RLE, 8 bit color
#Type 1 = 2 color      : 15 bits RLE, 1 bit color
#Type 2 = 4 color      : 14 bits RLE, 2 bit color
#Type 3 = 8 color      : 13 bits RLE, 3 bit color
#Type 4 = 16 color     : 12 bits RLE, 4 bit color
#Type 5 = 32 color     : 11 bits RLE, 5 bit color
#Type 6 = 64 color     : 10 bits RLE, 6 bit color
#Type 7 = 128 color    : 9  bits RLE, 7 bit color
#Type 8 = 256 color    : 8  bits RLE, 8 bit color

#If more pixels than RLE length can accomodate then just max first entry and repeat
#NB RLE length count +1, i.e. length 0 really has 1 pixel

#These should really be command lines...
my $BaseName = "Stop_black_8";
my $LeftClip = 0;
my $RightClip = 0;
my $TopClip = 0;
my $BottomClip = 0;
my $Compress = "Y";#"Y" or "N"
my $Filename = $BaseName . ".bmp";
my $OutFilename = $BaseName . ".c";
my $Width;
my $Height;
my $Depth;
my $X;
my $Y;
my $Data;
my $PixelColor;
my $XPos;
my $YPos;
my $FilePos;
my $ColorCount;
my $UniqueColors;
my $PixelCount;
my $ImageBuffer;
my $ImageBufferIndex = 0;
my $Palette;
my $PaletteCount = 0;
my $Type;
my $MaxRLE;
my $Count;
my $LastColor;
my $OutputCount = 0;
my $FinalSize;
my $Ratio;
my $OutputBuffer = "";

sub DataPack
{
    my $Count = $_[0];
    my $Color = $_[1];
    my $Type  = $_[2];
    return (($Count - 1) << $Type) | ($Color);
}

sub RGB_2_16
{
    my $R = ($_[0] >> 19) & 0x1f;# 5 upper bits
    my $G = ($_[0] >> 10) & 0x3f;# 6 upper bits
    my $B = ($_[0] >> 3)  & 0x1f;# 5 upper bits
    
    return ($R << 11) + ($G << 5) + ($B << 0)
}

sub ReadTripple
{
  my $T1;
  my $T2;
  my $T3;
  read (INHANDLE, $T1, 1);
  read (INHANDLE, $T2, 1);
  read (INHANDLE, $T3, 1);
  return ord($T1) + (ord($T2) * 256) + (ord($T3) * 256 * 256);
}

sub ReadDWord
{
  my $T1;
  my $T2;
  my $T3;
  my $T4;
  read (INHANDLE, $T1, 1);
  read (INHANDLE, $T2, 1);
  read (INHANDLE, $T3, 1);
  read (INHANDLE, $T4, 1);
  return ord($T1) + (ord($T2) * 256) + (ord($T3) * 256 * 256) + (ord($T4) * 256 * 256 * 256);
}

sub ReadWord
{
  my $T1;
  my $T2;
  read (INHANDLE, $T1, 1);
  read (INHANDLE, $T2, 1);
  return ord($T1) + (ord($T2) * 256);
}

sub ReadByte
{
  my $T1;
  read (INHANDLE, $T1, 1);
  return ord($T1);
}

sub LoadImage()
{
    my $BMPType;
    my $BMPSize;
    my $Res1;
    my $Res2;
    my$Offset;
    
    my $ColorTripplet;
    open(INHANDLE, "$Filename")|| die "Could not open '$Filename'";
    binmode INHANDLE;

    $BMPType = ReadWord();
    $BMPSize = ReadDWord();
    $Res1 = ReadWord();;
    $Res2 = ReadWord();;
    $Offset = ReadDWord();;
    
    seek (INHANDLE, 0x12, 0);
    $Width = ReadDWord();
    seek (INHANDLE, 0x16, 0);
    $Height = ReadDWord();
    seek (INHANDLE, 0x1C, 0);
    $Depth = ReadWord();
    
    if ($Depth != 24)
    {
        print "Error, color depth not set to 24 bits per color\n";
        exit;
    }
    
    print "Source Image Width  = $Width\n";
    print "Source Image Height = $Height\n";
    
    if (($Width & 3) != 0)
    {
      print "Image width must be multiple of 4";
      exit(-1);
    }

    seek (INHANDLE, 0x36, 0);
    #Scan the image and build a palette list & paletized image buffer
    for ($Y = 0; $Y < $Height; $Y++)
    {
        for ($X = $LeftClip; $X < $Width - $RightClip; $X++)
        {
            $FilePos = ($X + (($Height - 1 - $Y) * $Width));
#            $FilePos = $ImageBufferIndex;
            seek (INHANDLE, $Offset + 3 * $FilePos, 0);
            $ColorTripplet = ReadTripple();
            $ColorHex = sprintf("0x%06X", $ColorTripplet);
            $PixelColor = RGB_2_16($ColorTripplet);
            $PixelHex = sprintf("0x%04X", $PixelColor);
            if (($X == 12) and ($Y == 12))
            {
                $Steve++;
            }
            if (!exists $ColorCount{$PixelColor})#Color not yet declared so add as new entry
            {
                $Palette[$PaletteCount] = $PixelColor;
                $ColorToPalette{$PixelColor} = $PaletteCount;
                $PaletteCount++;
            }
            $ColorCount{$PixelColor}++;
            $ImageBuffer[$ImageBufferIndex] = $ColorToPalette{$PixelColor};
            $ImageBufferIndex++;
        }
    }
    #Correct limits for any cliping
    $Width = $Width - $LeftClip - $RightClip;
    $Height = $Height - $TopClip - $BottomClip;
    
    $UniqueColors = keys %ColorCount;
    $PixelCount = $Width *$Height;
    if ($Compress eq "N")
    {
        $Type = 0;
    }
    elsif    ($UniqueColors <= 2)
    {
        $Type = 1;
    }
    elsif ($UniqueColors <= 4)
    {
        $Type = 2;
    }
    elsif ($UniqueColors <= 8)
    {
        $Type = 3;
    }
    elsif ($UniqueColors <= 16)
    {
        $Type = 4;
    }
    elsif ($UniqueColors <= 32)
    {
        $Type = 5;
    }
    elsif ($UniqueColors <= 64)
    {
        $Type = 6;
    }
    elsif ($UniqueColors <= 128)
    {
        $Type = 7;
    }
    elsif ($UniqueColors <= 256)
    {
        $Type = 8;
    }
    else
    {
        print "Too many colors. Not currently supported\n";
        print "Unique colors = $UniqueColors\n";
        exit(-1);
    }
    close INHANDLE;

    print "Output Image Width  = $Width\n";
    print "Output Image Height = $Height\n";
}

sub BuildHeader()
{
    $OutputBuffer .= sprintf("#include <stdint.h>\n\n");
    $OutputBuffer .= sprintf("const uint16_t $BaseName" . "[] = {\n");
    $OutputBuffer .= sprintf("             0x%04X, //Width = %d\n", $Width, $Width);
    $OutputBuffer .= sprintf("             0x%04X, //Height = %d\n", $Height, $Height);
    $OutputBuffer .= sprintf("             0x%04X, //Type = %d, Colors = %d\n", ($Type * 256) + ($UniqueColors & 0xff), $Type, $UniqueColors);
    #Need to come back later and fill in the resultant size
    $OutputBuffer .= sprintf("##OUTPUTSIZE##\n");
}

sub AppendPalette()
{
    for ($PaletteCount = 0; $PaletteCount < $UniqueColors; $PaletteCount++)
    {
    #    printf("             0x%04X, //Color index %d\n", $Palette[$PaletteCount], $PaletteCount);
        $OutputBuffer .= sprintf("             0x%04X, //Color index %d occurs %d times\n", $Palette[$PaletteCount], $PaletteCount, $ColorCount{$Palette[$PaletteCount]});
    }
}

sub AppendRLEImage()
{
    $MaxRLE = 2 ** (15 - $Type);
    
    $Count = 1;
    $LastColor = $ImageBuffer[0];
    #Now scan the indexed buffer and do RLE
    for ($ImageBufferIndex = 1; $ImageBufferIndex < $PixelCount; $ImageBufferIndex++)#Count from 1 since 0 already counted above (LastColor)
    {
      if (($LastColor != $ImageBuffer[$ImageBufferIndex]) or ($Count == $MaxRLE))
      {
        #Pixel color different so send count so far and update current
    #    print"$LastColor : $Count\n";
        #printf("             0x%04X, //%d x color %d\n", DataPack($Count, $LastColor, $Type), $Count, $LastColor);
        $OutputBuffer .= sprintf("             0x%04X, //%d x color %d\n", DataPack($Count, $LastColor, $Type), $Count, $LastColor);
        $Count = 1;
        $LastColor = $ImageBuffer[$ImageBufferIndex];
        $OutputCount ++;
      }
      else
      {
        #Pixel color the same so increment count
        $Count++;   
      }
    }
    #Send remaining data
    #print"$LastColor : $Count\n";
    #printf("             0x%04X  //%d x color %d\n", DataPack($Count, $LastColor, $Type), $Count, $LastColor);
    #printf("              };\n");
    $OutputBuffer .= sprintf("             0x%04X  //%d x color %d\n", DataPack($Count, $LastColor, $Type), $Count, $LastColor);
    $OutputBuffer .= sprintf("              };\n");
    $OutputCount ++;
}

sub DisplaySummary()
{
    print "Pixel count = $PixelCount\n";
    print "Unique colors = $UniqueColors\n";
    $FinalSize = 3 + $UniqueColors + $OutputCount;
    print "Final image data size = $FinalSize data words\n";
    $Ratio = 100 * $FinalSize / $PixelCount;
    print "Compression ratio = $Ratio\%\n";
}

sub GenerateOutput()
{
    print "Creating output file $OutFilename\n";
    #Update the image data size
    $OutputSizeData = sprintf("             0x%04X, //Image words = %d", $OutputCount, $OutputCount);
    $OutputBuffer =~ s/##OUTPUTSIZE##/$OutputSizeData/;
    
    $OutputBuffer .= "\n";
    
    $OutFilename = $Filename;
    $OutFilename =~ s/\.bmp/\.c/;
    open(OUTHANDLE, ">", "$OutFilename")|| die "Could not open '$OutFilename'";
    printf(OUTHANDLE $OutputBuffer);
    close OUTHANDLE;
}

sub AppendRawImage()
{
    $ImageBufferIndex = 0;
    for ($Y = 0; $Y < $Height; $Y++)
    {
        for ($X = 0; $X < $Width; $X+=2)
        {
            if (($X == ($Width - 2)) and ($Y == ($Height - 1)))#Last entry so do not include ","
            {
                $OutputBuffer .= sprintf("             0x%02X%02X  //(%03d:%03d) index %d | index %d\n", $ImageBuffer[$ImageBufferIndex], $ImageBuffer[$ImageBufferIndex + 1], $X, $Y, $ImageBuffer[$ImageBufferIndex], $ImageBuffer[$ImageBufferIndex + 1]);
            }
            else
            {
                $OutputBuffer .= sprintf("             0x%02X%02X, //(%03d:%03d) index %d | index %d\n", $ImageBuffer[$ImageBufferIndex], $ImageBuffer[$ImageBufferIndex + 1], $X, $Y, $ImageBuffer[$ImageBufferIndex], $ImageBuffer[$ImageBufferIndex + 1]);
            }
            $ImageBufferIndex+=2;
            $OutputCount +=1;#Reducing from 16 bit (actually 24) down to 8 bit indexed so only 1 word per 2 pixels
        }
    }
    $OutputBuffer .= sprintf("             };\n\n");
}

###############################################################
###############################################################
##  Main script start                                        ##
###############################################################
###############################################################

LoadImage();
BuildHeader();
AppendPalette();

if ($Compress eq "Y")
{
    AppendRLEImage();
}
else
{
    AppendRawImage();
}

DisplaySummary();

GenerateOutput();