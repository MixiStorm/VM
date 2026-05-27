import Setings

# Linkerul trebuie sa ia codul sursa si sa il proceseze inlocuind unde trebuie adresele corecte 

class Linker:
    def __init__(self , L_Tokens):
        self.Lista_Tokens = L_Tokens
        self.Boot_Tokens = []
        self.Curent_Offset = Setings.CURENT_ADDR

        # Variabile pentru bootloader 
        self.boot_Start_addr = 0 
        self.boot_End_addr = 0 
        self.boot_Target_addr = Setings.BOOT_LOADER_SIZE
        self.mem_lock_start = 0 
        self.mem_lock_end = 0 

    def Proces_Labels(self):
        Source_Code = []
        i = 0 
        for  line in self.Lista_Tokens:
            new_line = []
            for tok in line :
                #print(f"[DEBUG] Procesare labels token : {tok}")
                if tok.endswith(":"):
                    Setings.LabelsDict[tok[:len(tok) - 1]] = i + self.Curent_Offset
                else:
                    new_line.append(tok)
            
            if new_line:
                Source_Code.append(new_line)
                i += 1
        self.Lista_Tokens = Source_Code
        
        #Trebuie sa verificam daca chiar exista labelul _start , pentru ca daca nu exista trebuie sa aruncam o eroare 
        if not "_start" in Setings.LabelsDict:
            raise ValueError("Nu exista labelul '_start' in source code")

    # Functia care va genera Boot_Loader-ul 
    def GenBoot(self):
        # Trebuie sa calculam intreaga dimensiune a programului 
        Program_Size = len(self.Lista_Tokens)
        self.boot_Start_addr = Setings.RAM_SIZE +  Setings.BOOT_LOADER_SIZE # Vrem sa incepem sa copiem datele din Rom doar de dupa boot_loader
        self.boot_End_addr = Setings.RAM_SIZE +  self.Curent_Offset + Program_Size
        self.mem_lock_start = self.Curent_Offset + 1
        self.mem_lock_end = self.Curent_Offset + Program_Size

        # Incepem sa generam lista de tokens (Corectat: adăugată virgula lipsă)
        self.Boot_Tokens = [
            f"SET R0 {self.boot_Start_addr}",
            f"SET R1 {self.boot_End_addr}",
            f"SET R2 {self.boot_Target_addr}",
            "ROM_READ R0 R1 R2",
            f"SET SP {self.mem_lock_end + 1}",
            f"SET R3 {self.mem_lock_start}",  # <-- Reparata virgula aici!
            f"SET R4 {self.mem_lock_end}",
            f"MEM_LOCK R3 R4",
            "JMP _start"
        ]

        # Tokenizam fiecare linie in parte
        b = []
        for i in range(len(self.Boot_Tokens)):
            line = self.Boot_Tokens[i].split()
            new_line = []
            for tok in line:
                new_line.append(tok)
            b.append(new_line)
        
        self.Boot_Tokens = b
    
    def NrToBin(self , nr):
        # Pentru un numar pe 64 de biti avem nevoie de un buffer de 8 octeti!
        # Folosim Little-Endian (octetul cel mai putin semnificativ primul), conform x86/x64 si C++ standard
        bin_nr = bytearray(8)
        mask = 255
        for i in range(8):
            bin_nr[i] = (nr >> (8 * i)) & mask
        return bin_nr

    def Pack(self , op , rx_1 , rx_2 , rx_3 , imm):
        inst = op & 0xFF
        inst = inst | ((rx_1 & 0x0F) << 8)
        inst = inst | ((rx_2 & 0x0F) << 12)
        inst = inst | ((rx_3 & 0x0F) << 16)
        inst = inst | ((imm & 0xFFFFFFFFFFF) << 20)
        
        # O instructiune in VM are 64 de biti, adica 8 octeti!
        ba = bytearray(8)
        maska = 255
        for i in range(8):
            ba[i] = (inst >> (8 * i)) & maska   
        return ba 

    def ToBin(self , Tokens):
        bloc_binar = bytearray()

        # Parcurgem TOATE liniile din Tokeni (Reparat: eliminat return-ul prematur)
        for line in Tokens:
            if not line:
                continue

            rx_1 = 0 
            rx_2 = 0 
            rx_3 = 0 
            imm = 0 
            
            # Preluam opcode-ul din prima pozitie a liniei
            if line[0].upper() == "STO":
                line[0] = "store"
            mnemonic = line[0].upper()
            if mnemonic in Setings.Opcodes:
                op = Setings.Opcodes[mnemonic]
            else:
                raise ValueError(f"Nu am detectat nici un opcode valid in aceasta instructiune : {line}")

            # Parcurgem argumentele ramase (de la indexul 1 incolo)
            for i in range(1, len(line)):
                tok = line[i]
                # Curatam virgulele ramase accidental de la tokenizare
                tok = tok.replace(",", "").strip()
                tok_upper = tok.upper()

                # Determinăm unde plasăm argumentul curent (al câtelea parametru este)
                if i == 1:
                    if tok_upper in Setings.Registri:
                        rx_1 = Setings.Registri[tok_upper]
                    elif tok.replace('-', '', 1).isdigit(): # Suport și pentru numere negative
                        imm = int(tok)
                    elif tok in Setings.LabelsDict:
                        imm = Setings.LabelsDict[tok]
                    else:
                        raise ValueError(f"Argument 1 invalid: {tok} in {line}")
                elif i == 2:
                    if tok_upper in Setings.Registri:
                        rx_2 = Setings.Registri[tok_upper]
                    elif tok.replace('-', '', 1).isdigit():
                        imm = int(tok)
                    elif tok in Setings.LabelsDict:
                        imm = Setings.LabelsDict[tok]
                    else:
                        raise ValueError(f"Argument 2 invalid: {tok} in {line}")
                elif i == 3:
                    if tok_upper in Setings.Registri:
                        rx_3 = Setings.Registri[tok_upper]
                    elif tok.replace('-', '', 1).isdigit():
                        imm = int(tok)
                    elif tok in Setings.LabelsDict:
                        imm = Setings.LabelsDict[tok]
                    else:
                        raise ValueError(f"Argument 3 invalid: {tok} in {line}")

            # Impachetam cei 8 octeti ai instructiunii si ii adaugam in blocul binar curent
            bloc_binar += self.Pack(op , rx_1 , rx_2 , rx_3 , imm)
            
        return bloc_binar

    def GenBin(self):
        full_bin = bytearray()
        boot_bin = bytearray()
        source_code_bin = bytearray()
        var_bin = bytearray()

        # Convertim codul pentru boot_loader si codul sursa in binar
        boot_bin = self.ToBin(self.Boot_Tokens)
        source_code_bin = self.ToBin(self.Lista_Tokens)

        # Adaugam binarul pentru variabile (Fiecare valoare/slot pe 8 octeti)
        for key , details in Setings.Variabile.items():
            for name , det in details.items():
                if key == "var":
                    var_bin += self.NrToBin(int(det["value"]))
                elif key == "arr":
                    for i in range(int(det["dimension"])):
                        var_bin += self.NrToBin(0)

        # Generam structura binarului final exact cum o vrea memoria
        full_bin += boot_bin 
        full_bin += var_bin
        full_bin += source_code_bin

        return full_bin

    def WriteBin(self):
        bin_data = self.GenBin() # Obtinem intreg binarul ca bytearray curat
        print(f"[LINKER] Dimensiune totala binar: {len(bin_data)} octeti ({len(bin_data) // 8} instructiuni/sloturi)")

        # Reparat: f.write() poate scrie direct un bytearray intreg, nu mai e nevoie sa buclam manual!
        with open(Setings.Binery_Path , "wb") as f:
            f.write(bin_data)
            
        print(f"Binarul a fost scris cu succes in fisierul : {Setings.Binery_Path}")    

    def Start(self):
        self.Proces_Labels() #Procesam labelurile 
        self.GenBoot() # Generam boot_loaderul 
        self.WriteBin() # Scriem totul pe disc