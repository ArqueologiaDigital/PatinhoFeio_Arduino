add_library('serial')

SMALL_LED = 8
SMALL_INC = 15

BIG_LED = 12
BIG_INC = 46

buff = []

leds = []

def red_ON():
  fill(255, 0, 0)


def red_OFF():
  fill(128, 0, 0)


def green_ON():
  fill(0, 255, 0)


def green_OFF():
  fill(0, 128, 0)


def white_ON():
  fill(255, 255, 255)


def white_OFF():
  fill(128, 128, 128)


def dados_painel(val):
    global leds
    base_x, base_y = 109, 474
    i = 0
    for a, b in enumerate(range(11, -1, -1)):
        leds[i+a] = ((val & (1 << a)) == (1 << a))
        if leds[i+a]:
            red_ON()
        else:
            red_OFF()
        ellipse(base_x + BIG_INC*b, base_y, BIG_LED, BIG_LED)
        
    base_x, base_y = 434, 277
    for a, b in enumerate(range(11, -1, -1)):
        if leds[i+a]:
            red_ON()
        else:
            red_OFF()
        ellipse(base_x + SMALL_INC*b, base_y, SMALL_LED, SMALL_LED)

        
def vai_um(val):
    base_x, base_y = 600, 170
    leds[12] = True if val == 1 else False
    if leds[12]:
        red_ON()
    else:
        red_OFF()
    ellipse(base_x , base_y, BIG_LED, BIG_LED)


def transbordo(val):
    base_x, base_y = 436, 170
    leds[13] = True if val == 1 else False
    if leds[13]:
        red_ON()
    else:
        red_OFF()
    ellipse(base_x , base_y, BIG_LED, BIG_LED)


def parado(val):
    base_x, base_y = 340, 378
    leds[14] = True if val == 1 else False
    if leds[14]:
        white_ON()
    else:
        white_OFF()
    ellipse(base_x , base_y, BIG_LED, BIG_LED)


def externo(val):
    base_x, base_y = 401, 378
    leds[15] = True if val == 1 else False
    if leds[15]:
        white_ON()
    else:
        white_OFF()
    ellipse(base_x , base_y, BIG_LED, BIG_LED)


#ci -> Endreço de Instrução
def ci(val):
    global leds
    base_x, base_y = 434, 121
    i = 16
    for a, b in enumerate(range(11, -1, -1)):
        leds[i+a] = ((val & (1 << a)) == (1 << a))
        if leds[i+a]:
            red_ON()
        else:
            red_OFF()
        ellipse(base_x + SMALL_INC*b, base_y, SMALL_LED, SMALL_LED)


# re -> Endereço na Memória
def re(val):
    global leds
    base_x, base_y = 434, 57
    i = 28
    for a, b in enumerate(range(11, -1, -1)):
        leds[i+a] = ((val & (1 << a)) == (1 << a))
        if leds[i+a]:
            red_ON()
        else:
            red_OFF()
        ellipse(base_x + SMALL_INC*b, base_y, SMALL_LED, SMALL_LED)


# rd -> Dados da Memória
def rd(val):
    global leds
    base_x, base_y = 146, 250
    i = 40
    for a, b in enumerate(range(7, -1, -1)):
        leds[i+a] = ((val & (1 << a)) == (1 << a))
        if leds[i+a]:
            red_ON()
        else:
            red_OFF()
        ellipse(base_x + SMALL_INC*b, base_y, SMALL_LED, SMALL_LED)


# ri -> Código de Instrução
def ri(val):
    global leds
    base_x, base_y = 146, 185
    i = 48
    for a, b in enumerate(range(7, -1, -1)):
        leds[i+a] = ((val & (1 << a)) == (1 << a))
        if leds[i+a]:
            red_ON()
        else:
            red_OFF()
        ellipse(base_x + SMALL_INC*b, base_y, SMALL_LED, SMALL_LED)


# acc -> Acumulador
def acc(val):
    global leds
    base_x, base_y = 146, 120
    i = 56
    for a, b in enumerate(range(7, -1, -1)):
        leds[i+a] = ((val & (1 << a)) == (1 << a))
        if leds[i+a]:
            red_ON()
        else:
            red_OFF()
        ellipse(base_x + SMALL_INC*b, base_y, SMALL_LED, SMALL_LED)

# TODO: FASE


def modo(val):
    global leds
    base_x, base_y = 79, 566
    i = 71
    s = 19
    inc = 58
    for a, b in enumerate(range(5, -1, -1)):
        leds[i+a] = ((val & (1 << a)) == (1 << a))
        if leds[i+a]:
            green_ON()
        else:
            green_OFF()
        ellipse(base_x + inc*b, base_y, s, s)


def espera(val):
    base_x, base_y = 525, 566
    s = 19
    leds[13] = True if val == 1 else False
    if leds[13]:
        green_ON()
    else:
        green_OFF()
    ellipse(base_x , base_y, s, s)


def interrupcao(val):
    base_x, base_y = 584, 566
    s = 19
    leds[13] = True if val == 1 else False
    if leds[13]:
        green_ON()
    else:
        green_OFF()
    ellipse(base_x , base_y, s, s)


def preparacao(val):
    base_x, base_y = 584+56, 620
    s = 19
    leds[13] = True if val == 1 else False
    if leds[13]:
        green_ON()
    else:
        green_OFF()
    ellipse(base_x , base_y, s, s)


def setup():
    #noStroke()
    size(729, 665)
    for i in range(80):
        leds.append(False)
    dados_painel(11)
    # print(leds)
    
    # print(Serial.list())
    portName = '/dev/ttyACM0' # Pick last serial port in list
    # try:
    #     myPort = Serial(this, portName, 115200)
    #     myPort.bufferUntil(10)
    # except:
    #     pass

    myPort = Serial(this, portName, 115200)
    myPort.bufferUntil(10)
    
    pato = loadImage("/tmp/pato.png")
    image(pato, 0, 0)
    frameRate(5)


def draw():
    global buff
    if len(buff) == 80:
        update_panel(buff)
    else:
        print 'There is not enough data to update panel'
    
    
def serialEvent(evt):
    global buff
    data = evt.readString()
    if data.startswith('LEDS:'):
        split = data.split(':')
        if len(split) == 2:
            buff = split[1].strip()
    elif data.startswith('TTY:'):
        split = data.split(':')
        if len(split) == 2:
            print 'Teletype: ', split[1].strip()


def binary_str_to_int(lst):
    if not lst:
        return 0
    return reduce(lambda x,y:x+y, [int(v)<<i for i, v in enumerate(lst)])


def update_panel(status):
    global leds
    dados_painel(binary_str_to_int(status[0:12]))
    vai_um(int(status[12]))
    transbordo(int(status[13]))
    parado(int(status[14]))
    externo(int(status[15]))
    ci(binary_str_to_int(status[16:28]))
    re(binary_str_to_int(status[28:40]))
    rd(binary_str_to_int(status[40:8]))
    ri(binary_str_to_int(status[48:56]))
    acc(binary_str_to_int(status[56:64]))
    modo(binary_str_to_int(status[71:77]))
    espera(int(status[77]))
    interrupcao(int(status[78]))
    preparacao(int(status[79]))