
def Convert():
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

#Acum trebuie sa creeam AST-ul 

expresie = "5 * ( 7 + 2 ) - ( 3 - 2 * 4 ) / 5".split()

print(expresie)
print(expresie[2:])


class BinOP:
	def __init__(self , stanga , op , dreapta):
		self.stanga = stanga
		self.op = op 
		self.dreapta = dreapta

# --- FUNCTIA DE AFISARE RECURSIVA ---
def afiseaza_ast(nod, prefix="", este_ultimul=True):
    """
    Afișează un AST în consolă sub formă de arbore ierarhic (text-graphics).
    """
    # Dacă nodul curent este o instanță de BinOP (adică este un operator)
    if isinstance(nod, BinOP):
        # Afișăm operatorul curent cu prefixul potrivit
        print(prefix + ("└── " if este_ultimul else "├── ") + nod.op)
        
        # Calculăm prefixul pentru copiii acestui nod
        nou_prefix = prefix + ("    " if este_ultimul else "│   ")
        
        # Apelăm recursiv pentru copilul stâng și cel drept
        # Copilul stâng NU este ultimul (urmează cel drept), deci este_ultimul=False
        afiseaza_ast(nod.stanga, nou_prefix, este_ultimul=False)
        # Copilul drept este ultimul din acest sub-arbore
        afiseaza_ast(nod.dreapta, nou_prefix, este_ultimul=True)
        
    else:
        # Dacă este doar un număr (valoare text/int), îl afișăm ca pe o frunză
        print(prefix + ("└── " if este_ultimul else "├── ") + str(nod))

def Parse(expr , poz = 0):

	#Puterea fiecarui poerator 
	operator_power = {
		"*": 2.0,
		"/": 1.9,
		"+": 1.0,
		"-": 0.9
	}
	

	#Daca este doar un numar il retrunam si gata , presupunand ca toate expresiile sunt scrise corect 
	if len(expr) == 1 :
		return expr[0]

	if expr[0] == '(' and expr[-1] == ')':
		# Verificăm totuși dacă paranteza de la început se închide chiar la sfârșit, nu pe parcurs
		contor = 0
		inchisa_la_final = True
		
		for i in range(len(expr) - 1):
		    if expr[i] == '(': contor += 1
		    if expr[i] == ')': contor -= 1
		    if contor == 0 and i > 0:
		        inchisa_la_final = False
		        break
		if inchisa_la_final:
		    return Parse(expr[1:-1])


	#Cautam cel mai important operator de pana acum 
	operatori = "+-*/"
	paranteze = "()"
	last_operator_power = 10  #Am pus un numar random pentru ca trebuie sa fie mai mari ca ori ce putere de operatori predefinita pana acum 
	poz = 0
	depth = 0


	for i in range(len(expr)):
		if expr[i] == "(":
			depth += 1

		elif expr[i] == ")":
			depth -= 1


		if expr[i] in operatori :
			"""
				Logica este in felul urmator 
					Daca puterea operatorului curent este mai mica ca ultimul il atribuim 
					Pe urma o sa verificam daca am ajuns la sfarsitul listei , daca da ne oprim si returnam 
			"""
			if operator_power[expr[i]] + (depth * 10 ) <= last_operator_power:
				last_operator_power = operator_power[expr[i]]
				poz = i

	stanga = expr[ :poz]
	dreapta = expr[poz + 1 : ]
	op = expr[poz]
	
	return BinOP(
		Parse(stanga),
		op ,
		Parse(dreapta)
		)


AST = Parse(expresie)
afiseaza_ast(AST)








