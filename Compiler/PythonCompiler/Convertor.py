

Enum_Registri = """
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
		RET = 0x1e,
		MEM_LOCK = 0x1f
        """

Enum_Registri = Enum_Registri.replace("," , " ").replace("=" , " ").split()
#Eliminam virgulele
New_enum_Registri= [] 
for e in Enum_Registri:
    if e != "," and e != "=":
        New_enum_Registri.append(e)


print(New_enum_Registri)
Nume_Dicitionare = "Registri = {\n"
for i in range( 0 , len(New_enum_Registri) , 2):
    Nume_Dicitionare += '"' + New_enum_Registri[i] + '"' + " : " + New_enum_Registri[i + 1] + ",\n"
Nume_Dicitionare += "}"

print(Nume_Dicitionare)
