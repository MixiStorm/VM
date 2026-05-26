import Setings

#Linkerul trebuie sa ia codul sursa si sa il proceseze inlocuind unde trebuie adresele corecte 

class Linker:
	def __init__(self , L_Tokens):
		self.Lista_Tokens = L_Tokens
		self.Boot_Tokens = []
		self.Lista_Final_Tokens = [] #Aici vom pune la final lista cu toate tokenurile
		self.Curent_Offset = Setings.CURENT_ADDR
		self.Corectare_Ofseturi_Labels()

		#Variabile pentru bootloader 
		self.boot_Start_addr = 0 
		self.boot_End_addr = 0 
		self.boot_Target_addr = Setings.BOOT_LOADER_SIZE
		self.mem_lock_start = 0 
		self.mem_lock_end = 0 

	def Corectare_Ofseturi_Labels(self):
		for key , value in Setings.LabelsDict.items():
			Setings.LabelsDict[key] = value + self.Curent_Offset


	#Functia care va genera Boot_Loader-ul 
	def GenBoot(self):
		#Trebuie sa calculam intreaga dimensiune a programului 
		Program_Size = len(self.Lista_Tokens)
		self.boot_Start_addr = Setings.BOOT_LOADER_SIZE #Vream sa incepem sa copiem datele din Rom doar de dupa boot_loader
		self.boot_End_addr = self.Curent_Offset + Program_Size
		self.mem_lock_start = self.Curent_Offset
		self.mem_lock_end = self.boot_End_addr

		#Incepem sa generam lista de tokens 
		self.Boot_Tokens = [
		f"set R0 {self.boot_Start_addr}"],
		f"set R1 {self.boot_End_addr}",
		f"set R2 {self.boot_Target_addr}",
		"rom_read R0 R1 R2",
		f"set SP {self.boot_End_addr}",
		f"set R3 {self.mem_lock_start}"
		f"set R4 {self.mem_lock_end}",
		f"mem_lock R3 R4",
		"jmp _start"
		]

		#Trebuie sa tokenizam fiecare linie imparte 
		b = []
		for i in range(len(self.Boot_Tokens)){
			line = self.Boot_Tokens[i].split()
			new_line = []
			for tok in line:
				new_line.append(tok)
			b.append(new_line)
		}
		self.Boot_Tokens = b


"""
	Cum va arata boot loaderul 
	std::vector<std::string> Boot_Loader = {
        "set R0 " + std::to_string(boot_Start_adress) , 
        "set R1 " + std::to_string(boot_End_adress),
        "set R2 " + std::to_string(boot_Target_adress),
        "rom_read R0 R1 R2" , 
        "set SP " + std::to_string(boot_Stack_init),
        "set R3 " + std::to_string(boot_Mem_lock_start),
        "set R4 " + std::to_string(boot_Mem_lock_end),
        "mem_lock R3 R4",
        "jmp _start"  
    };
"""