#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>

using namespace std;

#define ADDU 1
#define SUBU 3
#define AND 4
#define OR  5
#define NOR 7

// Memory size.
// In reality, the memory size should be 2^32, but for this lab and space reasons,
// we keep it as this large number, but the memory is still 32-bit addressable.
#define MemSize 65536

bitset<1> JudgeIsItype(const string& str);

class RF
{
public:
	bitset<64> ReadData1, ReadData2;
	RF()
	{
		Registers.resize(32);
		Registers[0] = bitset<64>(0);
	}

	void ReadWrite(bitset<5> RdReg1, bitset<5> RdReg2, bitset<5> WrtReg, bitset<64> WrtData, bitset<1> WrtEnable)
	{
		// TODO: implement!

		unsigned long r1 = RdReg1.to_ulong();
		unsigned long r2 = RdReg2.to_ulong();
		unsigned long w1 = WrtReg.to_ulong();

		ReadData1 = bitset<64>(Registers[r1].to_ulong());
		ReadData2 = bitset<64>(Registers[r2].to_ullong());

		if (WrtEnable[0])
		{
			Registers[w1] = WrtData;
		}
	}

	void OutputRF()
	{
		ofstream rfout;
		rfout.open("RFresult.txt", std::ios_base::app);
		if (rfout.is_open())
		{
			rfout << "A state of RF:" << endl;
			for (int j = 0; j < 32; j++)
			{
				rfout << Registers[j] << endl;
			}

		}
		else cout << "Unable to open file";
		rfout.close();

	}
private:
	vector<bitset<64> >Registers;
};


class ALU
{
public:
	bitset<64> ALUresult;
	bitset<64> ALUOperation(bitset<3> ALUOP, bitset<64> oprand1, bitset<64> oprand2)
	{
		// TODO: implement!
		unsigned long long aluop = ALUOP.to_ullong();
		long long result  = 0;
		if (aluop  == ADDU)
		{
			result  = oprand1.to_ullong() + oprand2.to_ullong();
		}
		else if (aluop  == SUBU)
		{
			result  = oprand1.to_ullong() - oprand2.to_ullong();
		}
		else if (aluop  == AND)
		{
			result  = oprand1.to_ullong() & oprand2.to_ullong();
		}
		else if (aluop  == OR)
		{
			result  = oprand1.to_ullong() | oprand2.to_ullong();
		}
		else if (aluop  == NOR)
		{
			result  = oprand1.to_ullong() ^ oprand2.to_ullong();
		}
		ALUresult  = bitset<64>(result);
		return ALUresult;
	}
};


class INSMem
{
public:
	bitset<32> Instruction;
	INSMem()
	{
		IMem.resize(MemSize);
		ifstream imem;
		string line;
		int i = 0;
		imem.open("imem.txt");
		if (imem.is_open())
		{
			while (getline(imem, line))
			{
				IMem[i] = bitset<8>(line.substr(0, 8));
				i++;
			}

		}
		else cout << "Unable to open file";
		imem.close();

	}

	bitset<32> ReadMemory(bitset<32> ReadAddress)
	{
		// TODO: implement!
		// (Read the byte at the ReadAddress and the following three byte).

		unsigned long offset = ReadAddress.to_ulong();

		string str = string("");
		for (size_t i = 0; i < 4; i++)
		{
			str += IMem[offset + i].to_string();
		}

		Instruction = bitset<32>(str);

		return Instruction;
	}

private:
	vector<bitset<8> > IMem;

};

class DataMem
{
public:
	bitset<64> readdata;
	DataMem()
	{
		DMem.resize(MemSize);
		ifstream dmem;
		string line;
		int i = 0;
		dmem.open("dmem.txt");
		if (dmem.is_open())
		{
			while (getline(dmem, line))
			{
				DMem[i] = bitset<8>(line.substr(0, 8));
				i++;
			}
		}
		else cout << "Unable to open file";
		dmem.close();

	}
	bitset<32> MemoryAccess(bitset<64> Address, bitset<64> WriteData, bitset<1> readmem, bitset<1> writemem)
	{
		// TODO: implement!

		unsigned long offset = Address.to_ulong();

		if (readmem[0])
		{
			string str = string("");
			for (size_t i = 0; i < 4; i++)
			{
				str += DMem[offset + i].to_string();
			}
			readdata = bitset<64>(str);
		}
		else if (writemem[0])
		{
			string str = WriteData.to_string();

			for (size_t i = 0; i < 4; i++)
			{
				string bt = str.substr(0, 8);
				DMem[offset + i] = bitset<8>(bt);
				str = str.substr(8);
			}
		}

		return NULL;
	}

	void OutputDataMem()
	{
		ofstream dmemout;
		dmemout.open("dmemresult.txt");
		if (dmemout.is_open())
		{
			for (int j = 0; j < 1000; j++)
			{
				dmemout << DMem[j] << endl;
			}

		}
		else cout << "Unable to open file";
		dmemout.close();

	}

private:
	vector<bitset<8> > DMem;

};


int main()
{
	RF myRF;
	ALU myALU;
	INSMem myInsMem;
	DataMem myDataMem;

	// Control Registers
	bitset<32> PC;
	bitset<1> wrtEnable;
	bitset<1> isJType;
	bitset<1> isIType;
	bitset<1> isLoad;
	bitset<1> isStore;
	bitset<1> isBranch;
	bitset<3> aluOp;

	while (1)
	{
		// 1. Fetch Instruction
		bitset<32> instruction = myInsMem.ReadMemory(PC);

		// If current insturciton is "11111111111111111111111111111111", then break;
		if (myInsMem.Instruction.to_ulong() == 0xffffffff) {
			break;
		}

		// decode(Read RF)
		// Decoder
		isLoad = instruction.to_string().substr(25, 7) == string("0000011");
		isStore = instruction.to_string().substr(25, 7) == string("0100011");
		isJType = instruction.to_string().substr(25, 7) == string("1101111");
		isBranch = instruction.to_string().substr(25, 7) == string("1100011");
		isIType = JudgeIsItype(instruction.to_string());
		wrtEnable = !(isStore.to_ulong() || isBranch.to_ulong() || isJType.to_ulong());
		aluOp = bitset<3>(instruction.to_string().substr(12, 3));

		// 2. Register File Instruction
		bitset<5> rs1 = bitset<5>(instruction.to_string().substr(15, 7));
		bitset<5> rs2 = isIType[0] ? bitset<5>(0) : bitset<5>(instruction.to_string().substr(20, 7));
		bitset<5> rd = bitset<5>(instruction.to_string().substr(7, 7));
		myRF.ReadWrite(rs1, rs2, rd, bitset<64>(0), wrtEnable);


		// 3. Execuete alu operation
		bitset<64> tmp(instruction.to_string().substr(20, 12)); // if positive, 0 padded
		if (tmp[31] == true) {
			tmp = bitset<64>(string(52, '1') + tmp.to_string().substr(20, 12));
		}
		myALU.ALUOperation(aluOp, myRF.ReadData1, isIType[0] ? tmp : myRF.ReadData2);


		// 4. Read/Write Mem(Memory Access)
		myDataMem.MemoryAccess(myALU.ALUresult, myRF.ReadData2, isLoad, isStore);

		// 5. Register File Update(Write Back)
		myRF.ReadWrite(rs1, rs2, rd, isLoad[0] ? myDataMem.readdata : myALU.ALUresult, wrtEnable);

		// Update PC
		if (isBranch[0] && myRF.ReadData1 == myRF.ReadData2) {
			bitset<32> addressExtend;
			if (instruction[15] == true) {
				addressExtend = bitset<32>(string(18, '1') + instruction.to_string().substr(31, 1) + instruction.to_string().substr(7, 1) + instruction.to_string().substr(25, 6) + instruction.to_string().substr(8, 4) + string("00"));
			}
			else {
				addressExtend = bitset<32>(string(18, '0') + instruction.to_string().substr(31, 1) + instruction.to_string().substr(7, 1) + instruction.to_string().substr(25, 6) + instruction.to_string().substr(8, 4) + string("00"));
			}
			PC = bitset<32>(PC.to_ulong() + 4 + addressExtend.to_ulong());
		}
		else if (isJType[0]) {
			bitset<32> addressExtend;
			addressExtend = bitset<32>(string(10, '0') + instruction.to_string().substr(31, 1) + instruction.to_string().substr(12, 8) + instruction.to_string().substr(20, 1) + instruction.to_string().substr(21, 10) + string("00"));
			PC = bitset<32>(PC.to_ulong() + 4 + addressExtend.to_ulong());
		}
		else {
			PC = bitset<32>(PC.to_ulong() + 4);
		}

		myRF.OutputRF(); // dump RF;        
	}
	myDataMem.OutputDataMem(); // dump data mem

	return 0;
}


bitset<1> JudgeIsItype(const string& str)
{
	bitset<1> result;
	string temp = str.substr(25, 7);
	result = (temp == string("1100111")) || (temp == string("0000011")) || (temp == string("0010011")) || (temp == string("0011011")) || (temp == string("0001111"));
	if (result.to_ulong())
		return result;
	else if (str.substr(25, 7) == string("1010011"))
	{
		result = (str.substr(11, 5)) == string("00000") || str.substr(11, 5) == string("000001");
	}
	return result;
}
