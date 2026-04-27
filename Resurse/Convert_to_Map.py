enum = """
		Add = 0x01, 
		SUB = 0x02,
		CMP = 0x03,
		SET = 0x04,
		LOAD = 0x05,
		STORE = 0x06,
		HALT = 0x07,
		ROM_Read = 0x08,
		ROM_Write = 0x09,
		MOV = 0x0A,
		PUSH = 0x0B,
		POP = 0x0C,
		NOP = 0x0D,
		JMP = 0x0f,
		JMQ = 0x10,
		JML = 0x11,
		JMM = 0x12,
        MUL = 0x13,
        DIV = 0x14,
        INC = 0x15,
        DEC = 0x16,
		XOR = 0x17,
		AND = 0x18,
		OR  = 0x19,
		NOT = 0x1a,
		SHL = 0x1b,
		SHR = 0x1c,
		CALL = 0x1d,
		RET = 0x1e
        """

enum = enum.replace('\n', "") 
enum = enum.replace("\r", "") 
enum = enum.replace("\t", "") 
enum = enum.replace(" ", "") 

s_pass = enum.split(",")
for i in range(len(s_pass)):
    s_pass[i] = s_pass[i].split("=")

#print(s_pass)

string_final = []

sf = ""

for s in s_pass:
    sf = '{"' + s[0].lower() + '", ' +s[1]+"}, "
    string_final.append(sf)
    sf = ""

for i in range(len(string_final)):
    if i % 4 == 0 and i != 0:
        print()
    print(string_final[i] , end="")
