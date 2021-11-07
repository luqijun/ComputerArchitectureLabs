#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
using namespace std;
#define MemSize 1000 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.

#define BitWidth 32

int i;

struct IFStruct {
	bitset<32>  PC;
	bool        nop;
};

struct IDStruct {
	bitset<32>  Instr;
	bool        nop;
};

struct EXStruct {
	bitset<64>  Read_data1;
	bitset<64>  Read_data2;
	bitset<16>  Imm;
	bitset<5>   Rs;
	bitset<5>   Rt;
	bitset<5>   Wrt_reg_addr;
	bool        is_I_type;
	bool        rd_mem;
	bool        wrt_mem;
	bool        alu_op;     //1 for addu, lw, sw, 0 for subu 
	bool        wrt_enable;
	bool        nop;
};

struct MEMStruct {
	bitset<64>  ALUresult;
	bitset<64>  Store_data;
	bitset<5>   Rs;
	bitset<5>   Rt;
	bitset<5>   Wrt_reg_addr;
	bool        rd_mem;
	bool        wrt_mem;
	bool        wrt_enable;
	bool        nop;
};

struct WBStruct {
	bitset<64>  Wrt_data;
	bitset<5>   Rs;
	bitset<5>   Rt;
	bitset<5>   Wrt_reg_addr;
	bool        wrt_enable;
	bool        nop;
};

struct stateStruct {
	IFStruct    IF;
	IDStruct    ID;
	EXStruct    EX;
	MEMStruct   MEM;
	WBStruct    WB;
};

class RF
{
public:
	bitset<64> Reg_data;
	RF()
	{
		Registers.resize(32);
		Registers[0] = bitset<64>(0);
	}

	bitset<64> readRF(bitset<5> Reg_addr)
	{
		Reg_data = Registers[Reg_addr.to_ulong()];
		return Reg_data;
	}

	void writeRF(bitset<5> Reg_addr, bitset<64> Wrt_reg_data)
	{
		Registers[Reg_addr.to_ulong()] = Wrt_reg_data;
	}

	void outputRF()
	{
		ofstream rfout;
		rfout.open("RFresult.txt", std::ios_base::app);
		if (rfout.is_open())
		{
			rfout << "State of RF:\t" << endl;
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
				IMem[i] = bitset<8>(line);
				i++;
			}
		}
		else cout << "Unable to open file";
		imem.close();
	}

	bitset<32> readInstr(bitset<32> ReadAddress)
	{
		string insmem;
		insmem.append(IMem[ReadAddress.to_ulong()].to_string());
		insmem.append(IMem[ReadAddress.to_ulong() + 1].to_string());
		insmem.append(IMem[ReadAddress.to_ulong() + 2].to_string());
		insmem.append(IMem[ReadAddress.to_ulong() + 3].to_string());
		Instruction = bitset<32>(insmem);		//read instruction memory
		return Instruction;
	}

private:
	vector<bitset<8> > IMem;
};

class DataMem
{
public:
	bitset<64> ReadData;
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
				DMem[i] = bitset<8>(line);
				i++;
			}
		}
		else cout << "Unable to open file";
		dmem.close();
	}

	bitset<64> readDataMem(bitset<32> Address)
	{
		string datamem;
		datamem.append(DMem[Address.to_ulong()].to_string());
		datamem.append(DMem[Address.to_ulong() + 1].to_string());
		datamem.append(DMem[Address.to_ulong() + 2].to_string());
		datamem.append(DMem[Address.to_ulong() + 3].to_string());
		datamem.append(DMem[Address.to_ulong() + 4].to_string());
		datamem.append(DMem[Address.to_ulong() + 5].to_string());
		datamem.append(DMem[Address.to_ulong() + 6].to_string());
		datamem.append(DMem[Address.to_ulong() + 7].to_string());
		ReadData = bitset<64>(datamem);		//read data memory
		return ReadData;
	}

	void writeDataMem(bitset<32> Address, bitset<64> WriteData)
	{
		DMem[Address.to_ulong()] = bitset<8>(WriteData.to_string().substr(0, 8));
		DMem[Address.to_ulong() + 1] = bitset<8>(WriteData.to_string().substr(8, 8));
		DMem[Address.to_ulong() + 2] = bitset<8>(WriteData.to_string().substr(16, 8));
		DMem[Address.to_ulong() + 3] = bitset<8>(WriteData.to_string().substr(24, 8));
		DMem[Address.to_ulong() + 4] = bitset<8>(WriteData.to_string().substr(32, 8));
		DMem[Address.to_ulong() + 5] = bitset<8>(WriteData.to_string().substr(40, 8));
		DMem[Address.to_ulong() + 6] = bitset<8>(WriteData.to_string().substr(48, 8));
		DMem[Address.to_ulong() + 7] = bitset<8>(WriteData.to_string().substr(56, 8));
	}

	void outputDataMem()
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

void printState(stateStruct state, int cycle)
{
	ofstream printstate;
	printstate.open("stateresult.txt", std::ios_base::app);
	if (printstate.is_open())
	{
		printstate << "State after executing cycle:\t" << cycle << endl;

		printstate << "IF.PC:\t" << state.IF.PC.to_ulong() << endl;
		printstate << "IF.nop:\t" << state.IF.nop << endl;

		printstate << "ID.Instr:\t" << state.ID.Instr << endl;
		printstate << "ID.nop:\t" << state.ID.nop << endl;

		printstate << "EX.Read_data1:\t" << state.EX.Read_data1 << endl;
		printstate << "EX.Read_data2:\t" << state.EX.Read_data2 << endl;
		printstate << "EX.Imm:\t" << state.EX.Imm << endl;
		printstate << "EX.Rs:\t" << state.EX.Rs << endl;
		printstate << "EX.Rt:\t" << state.EX.Rt << endl;
		printstate << "EX.Wrt_reg_addr:\t" << state.EX.Wrt_reg_addr << endl;
		printstate << "EX.is_I_type:\t" << state.EX.is_I_type << endl;
		printstate << "EX.rd_mem:\t" << state.EX.rd_mem << endl;
		printstate << "EX.wrt_mem:\t" << state.EX.wrt_mem << endl;
		printstate << "EX.alu_op:\t" << state.EX.alu_op << endl;
		printstate << "EX.wrt_enable:\t" << state.EX.wrt_enable << endl;
		printstate << "EX.nop:\t" << state.EX.nop << endl;

		printstate << "MEM.ALUresult:\t" << state.MEM.ALUresult << endl;
		printstate << "MEM.Store_data:\t" << state.MEM.Store_data << endl;
		printstate << "MEM.Rs:\t" << state.MEM.Rs << endl;
		printstate << "MEM.Rt:\t" << state.MEM.Rt << endl;
		printstate << "MEM.Wrt_reg_addr:\t" << state.MEM.Wrt_reg_addr << endl;
		printstate << "MEM.rd_mem:\t" << state.MEM.rd_mem << endl;
		printstate << "MEM.wrt_mem:\t" << state.MEM.wrt_mem << endl;
		printstate << "MEM.wrt_enable:\t" << state.MEM.wrt_enable << endl;
		printstate << "MEM.nop:\t" << state.MEM.nop << endl;

		printstate << "WB.Wrt_data:\t" << state.WB.Wrt_data << endl;
		printstate << "WB.Rs:\t" << state.WB.Rs << endl;
		printstate << "WB.Rt:\t" << state.WB.Rt << endl;
		printstate << "WB.Wrt_reg_addr:\t" << state.WB.Wrt_reg_addr << endl;
		printstate << "WB.wrt_enable:\t" << state.WB.wrt_enable << endl;
		printstate << "WB.nop:\t" << state.WB.nop << endl;
	}
	else cout << "Unable to open file";
	printstate.close();
}


int main()
{

	RF myRF;
	INSMem myInsMem;
	DataMem myDataMem;
	struct stateStruct state { 0 };
	state.IF.nop = false;
	state.ID.nop = true;
	state.EX.nop = true;
	state.MEM.nop = true;
	state.WB.nop = true;
	state.EX.alu_op = true;
	int cycle = 0;
	stateStruct newState;

	bool isRunning = false;

	//nop可理解为执行空指令
	while (1) {


		newState = state;

		/* --------------------- WB stage --------------------- */
		if (!newState.WB.nop) {

			if (newState.WB.wrt_enable) {
				myRF.writeRF(newState.WB.Wrt_reg_addr, newState.WB.Wrt_data);
			}

			if (newState.MEM.nop) {
				newState.WB.nop = true;
			}
			else {
				newState.WB.nop = false;
			}
		}

		/* --------------------- MEM stage --------------------- */
		if (!newState.MEM.nop) {

			newState.MEM.nop = true; // done this turn MEM
			newState.WB.nop = false; // activate WB in next turn
			newState.WB.Wrt_data = newState.MEM.ALUresult;// if need to wrt, get data
			newState.WB.Rs = newState.MEM.Rs; //if need Rs, pass the message to wb
			newState.WB.Rt = newState.MEM.Rt; // if need Rr, pass the message to rt
			newState.WB.Wrt_reg_addr = newState.MEM.Wrt_reg_addr; //if have wrt addr
			newState.WB.wrt_enable = newState.MEM.wrt_enable;

			if (newState.MEM.rd_mem && newState.WB.wrt_enable) {
				newState.WB.Wrt_data = myDataMem.readDataMem(bitset<32>(newState.MEM.ALUresult.to_string()));// get read data in
			}

			if (newState.MEM.wrt_mem) {
				myDataMem.writeDataMem(bitset<32>(newState.MEM.ALUresult.to_string()), newState.MEM.Store_data);//get wrt date in
			}

			if (newState.EX.nop) {
				newState.MEM.nop = true; //close MEM in next turn
			}
		}

		/* --------------------- EX stage --------------------- */

		//数据冲突：Ex阶段可能用到MEM中将要写回寄存器的数据
		if (!state.MEM.nop && state.MEM.wrt_enable) {
			cycle += 1;
			state = newState;
			cout << ("Wait For MEM Stage To Wirte Back, To Solve EX Stage Conflict...\r\n");
			continue;
		}


		if (!newState.EX.nop) {

			newState.EX.nop = true;
			newState.MEM.nop = false;
			newState.MEM.Store_data = newState.EX.Read_data1;
			newState.MEM.Rs = newState.EX.Rs;
			newState.MEM.Rt = newState.EX.Rt;
			newState.MEM.Wrt_reg_addr = newState.EX.Wrt_reg_addr;
			newState.MEM.rd_mem = newState.EX.rd_mem;
			newState.MEM.wrt_mem = newState.EX.wrt_mem;
			newState.MEM.wrt_enable = newState.EX.wrt_enable;

			if (newState.ID.nop) {
				newState.EX.nop = true;
			}

			if (newState.EX.alu_op) {
				//addu
				if (!newState.EX.is_I_type && !newState.EX.rd_mem) {
					newState.EX.Read_data1 = bitset<64>(myRF.readRF(newState.EX.Rt));  //rs1
					newState.EX.Read_data2 = bitset<64>(myRF.readRF(newState.EX.Rs));  //rs2
					newState.MEM.ALUresult = bitset<64>(newState.EX.Read_data1.to_ulong() + newState.EX.Read_data2.to_ulong());
				}
				//ld 
				else if (newState.EX.is_I_type) {
					newState.EX.Read_data1 = bitset<64>(myRF.readRF(newState.EX.Rt));  //rs1
					newState.EX.Read_data2 = bitset<64>(newState.EX.Imm.to_ulong());   //Imm
					newState.MEM.ALUresult = bitset<64>(newState.EX.Read_data1.to_ulong() + newState.EX.Read_data2.to_ulong());
				}
				//sd
				else {
					newState.EX.Read_data1 = bitset<64>(myRF.readRF(newState.EX.Rt));  //rs1
					newState.EX.Read_data2 = bitset<64>(newState.EX.Imm.to_ulong());   //Imm
					newState.MEM.ALUresult = bitset<64>(newState.EX.Read_data1.to_ulong() + newState.EX.Read_data2.to_ulong());
					if (!newState.EX.is_I_type) {
						newState.MEM.Store_data = bitset<64>(myRF.readRF(newState.EX.Rs));  //rs2
					}
				}
			}
			//subu
			else {
				newState.EX.Read_data1 = bitset<64>(myRF.readRF(newState.EX.Rt));  //rs1
				newState.EX.Read_data2 = bitset<64>(myRF.readRF(newState.EX.Rs));  //rs2
				newState.MEM.ALUresult = bitset<64>(newState.EX.Read_data1.to_ulong() - newState.EX.Read_data2.to_ulong());
			}
		}

		/* --------------------- ID stage --------------------- */

		//数据冲突处理  这里使用state而不是newState
		if (!state.WB.nop && state.WB.wrt_enable) {
			cycle += 1;
			state = newState;
			cout << ("Wait For WB Stage Processing, To Solve ID Stage Conflict...\r\n");
			continue;
		}

		//解析指令
		if (!newState.ID.nop) {

			newState.ID.Instr = myInsMem.Instruction;

			//func7 rs2 rs2 func3 rd opcode
			string func3 = newState.ID.Instr.to_string().substr(17, 3);
			string func7 = newState.ID.Instr.to_string().substr(0, 7);
			string opcode = newState.ID.Instr.to_string().substr(25, 7);
			string rs2 = newState.ID.Instr.to_string().substr(7, 5);
			string rs1 = newState.ID.Instr.to_string().substr(12, 5);
			string rd = newState.ID.Instr.to_string().substr(20, 5);

			//sub 和 add 的func3都为“000”
			if (func3 == "000")
			{
				//sub
				if (func7 == "0100000")
				{
					newState.EX.alu_op = false;
					newState.EX.Rs = bitset<5>(rs2);            //rs2
					newState.EX.Rt = bitset<5>(rs1);            //rs1
					newState.EX.Wrt_reg_addr = bitset<5>(rd);   //rd  目标寄存器地址
					newState.EX.rd_mem = false;
					newState.EX.wrt_enable = true;//写回寄存器
					newState.EX.is_I_type = false;
				}
				//add
				else
				{
					newState.EX.alu_op = true;
					//add
					string rd = newState.ID.Instr.to_string().substr(20, 5);
					newState.EX.Rs = bitset<5>(rs2);           //rs2
					newState.EX.Rt = bitset<5>(rs1);           //rs1
					newState.EX.Wrt_reg_addr = bitset<5>(rd);  //rd  目标寄存器地址
					newState.EX.rd_mem = false;
					newState.EX.wrt_mem = false;
					newState.EX.wrt_enable = true;//写回寄存器
					newState.EX.is_I_type = false;
				}
			}
			//ld 通过func3和opcode来判断
			else if (func3 == "011" && opcode == "0000011") {
				newState.EX.Imm = bitset<16>(func7 + rs2);          //Imm  立即数
				newState.EX.Rt = bitset<5>(rs1);           //rs1
				newState.EX.Wrt_reg_addr = bitset<5>(rd);  //rd   目标寄存器地址
				newState.EX.rd_mem = true;
				newState.EX.wrt_mem = false;
				newState.EX.wrt_enable = true;//写回寄存器
				newState.EX.is_I_type = true;
			}
			//sd 通过func3和opcode来判断
			else /*if (opcode == "0100011")*/ {
				newState.EX.Rs = bitset<5>(rs2);            //rs2
				newState.EX.Rt = bitset<5>(rs1);            //rs1
				newState.EX.Imm = bitset<16>(func7 + rd);   //Imm 由func7和rd共同组成
				newState.EX.rd_mem = true;
				newState.EX.wrt_mem = true;
				newState.EX.wrt_enable = false;//不需要写回寄存器
				newState.EX.is_I_type = false;
			}

			if (newState.IF.nop) {
				newState.ID.nop = true;
				newState.EX.nop = false;
			}
			else {
				newState.EX.nop = false;
			}
		}

		/* --------------------- IF stage --------------------- */
		if (!newState.IF.nop) {
			myInsMem.Instruction = myInsMem.readInstr(newState.IF.PC);

			cout << "\r\nCurrent Instruction: " + newState.ID.Instr.to_string() + "\r\n";

			if (myInsMem.Instruction.to_string() == "11111111111111111111111111111111") {

				if (!isRunning)
					continue;
				else {
					newState.ID.nop = true;
					newState.IF.nop = true;//第二次遇到“FFFFFFFF” 结束流水线取指令
				}
			}
			else {
				newState.ID.nop = false;
				newState.IF.PC = bitset<32>(newState.IF.PC.to_ulong() + 4);

				cout << "PC Value: " + newState.IF.PC.to_string() + "\r\n";
			}

			isRunning = true;
		}

		/* --------------------- Stall unit--------------------- */

		if (newState.IF.nop && newState.ID.nop && newState.EX.nop && newState.MEM.nop && newState.WB.nop)
			break;

		printState(newState, cycle); //print states after executing cycle 0, cycle 1, cycle 2 ... 

		cycle += 1;
		state = newState; /*The end of the cycle and updates the current state with the values calculated in this cycle */
	}

	myRF.outputRF(); // dump RF;	
	myDataMem.outputDataMem(); // dump data mem 
	return 0;
}
