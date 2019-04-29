#include <conio.h>
#include <stdio.h>
#include <stdlib.h>

struct Result {
	int value;
	int div0;
	int amp;
};


Result calc(Result rs1, Result rs2, char op) {
	Result rs;
	rs.value = 0;
	rs.amp = 1;
	rs.div0 = 0;
	if (rs1.div0==1 || rs2.div0==1) {
		rs.div0 = 1;
		return rs;
	}
	switch (op) {
		case '+': 
			if (rs1.amp != rs2.amp) {
				rs.value = rs1.value * rs2.amp + rs2.value * rs1.amp;
				rs.amp = rs1.amp * rs2.amp;
			}
			else {
				rs.value = rs1.value + rs2.value;
				rs.amp = rs1.amp;
			}
			break;
		case '-':
			if (rs1.amp != rs2.amp) {
				rs.value = rs1.value * rs2.amp - rs2.value * rs1.amp;
				rs.amp = rs1.amp * rs2.amp;
			}
			else {
				rs.value = rs1.value - rs2.value;
				rs.amp = rs1.amp;
			}
			break;
		case '*':
			rs.value = rs1.value * rs2.value;
			rs.amp = rs1.amp * rs2.amp;
			break;
		case '/':
			if (rs2.value==0) {
				rs.div0 = 1;
				return rs;
			}
			else {
				if (!(rs1.value%rs2.value)) {	//正好可整除
					rs.value = rs1.value * rs2.amp / rs2.value;
					rs.amp = rs1.amp;
				}
				else {
					rs.value = rs1.value * rs2.amp;
					rs.amp = rs1.amp * rs2.value;
				}
			}
			break;	
		}
	return rs;
}

void p24_main() {
	
	int i, maxnum = 10;
	int i_da, j_da, k_da, m_da;
	int i_op, j_op, k_op;
	int counter;	//成功次数
	int data_pre[4] = {0, 0, 0, 0};
	Result data_list[4];
	Result data[4];
	Result rst;
	char op_list[4] = {'+', '-', '*', '/'};
	char op[3];
	char sel;
	
	puts("Do you want to input number by yourself ? [y/n] N");
	sel = getch();
	if (sel=='y' || sel=='Y') {
		puts("Please input your number, seperate with SPACE, ENTER for confirm");
		scanf("%d%d%d%d", &data_pre[0], &data_pre[1], &data_pre[2], &data_pre[3]);
	}
	
	randomize();
	for (i=0; i<4; i++) {
		if (sel=='Y' || sel=='y') data_list[i].value = data_pre[i];
		else data_list[i].value = random(maxnum);
		data_list[i].div0 = 0;
		data_list[i].amp = 1;
	}
	
	printf("%d, %d, %d, %d\n\n", data_list[0].value, data_list[1].value, data_list[2].value, data_list[3].value);
	// 5*(5-(1/5))
	//rst = calc(data_list[0], calc(data_list[1], calc(data_list[3], data_list[2], '/'), '-'), '*');
	//printf("rst.value=%d, rst.div0=%d, rst.amp=%d \n", rst.value, rst.div0, rst.amp);
	
	for (i_da=0; i_da<4; i_da++) {	//排列
		data[0] = data_list[i_da];
		for (j_da=0; j_da<4; j_da++) {
			if (j_da==i_da) {continue;}
			data[1] = data_list[j_da];
			for (k_da=0; k_da<4; k_da++) {
				if (k_da==i_da || k_da==j_da) {continue;}
				data[2] = data_list[k_da];
				for (m_da=0; m_da<4; m_da++) {
					if (m_da==i_da || m_da==j_da || m_da==k_da) {continue;}
					data[3] = data_list[m_da];
					
					/**测试
					printf("%d %d %d %d\n",data[0].value, data[1].value, data[2].value, data[3].value);
					if (getch()==27) exit(0);
					**/
					
					//分配操作符
					for (i_op=0; i_op<4; i_op++) {
						op[0] = op_list[i_op];
						for (j_op=0; j_op<4; j_op++) {
							op[1] = op_list[j_op];
							for (k_op=0; k_op<4; k_op++) {
								op[2] = op_list[k_op];
								//计算
								//1. A ? ((B ? C) ? D)
								rst = calc(data[0], calc(calc(data[1], data[2], op[1]), data[3], op[2]), op[0]);
								if ((rst.value%rst.amp)==0 && rst.div0==0 && (rst.value/rst.amp)==24) {
									counter++;
									printf("%d%c((%d%c%d)%c%d)=%d\n",data[0].value,op[0],data[1].value,op[1],data[2].value,op[2],data[3].value,rst.value/rst.amp);
									//printf("rst.div0=%d, rst.amp=%d \n", rst.div0, rst.amp);
								}
								//2. A ? (B ? (C ? D))
								rst = calc(data[0], calc(data[1], calc(data[2], data[3], op[2]), op[1]), op[0]);
								if ((rst.value%rst.amp)==0 && rst.div0==0 && (rst.value/rst.amp)==24) {
									counter++;
									printf("%d%c(%d%c(%d%c%d))=%d\n",data[0].value,op[0],data[1].value,op[1],data[2].value,op[2],data[3].value,rst.value/rst.amp);
								}
								//3. (A ? B) ? (C ? D)
								rst = calc(calc(data[0], data[1], op[0]), calc(data[2], data[3], op[2]), op[1]);
								if ((rst.value%rst.amp)==0 && rst.div0==0 && (rst.value/rst.amp)==24) {
									counter++;
									printf("(%d%c%d)%c(%d%c%d)=%d\n",data[0].value,op[0],data[1].value,op[1],data[2].value,op[2],data[3].value,rst.value/rst.amp);
								}
								//4. ((A ? B) ? C) ? D
								rst = calc(calc(calc(data[0], data[1], op[0]), data[2], op[1]), data[3], op[2]);
								if ((rst.value%rst.amp)==0 && rst.div0==0 && (rst.value/rst.amp)==24) {
									counter++;
									printf("((%d%c%d)%c%d)%c%d=%d\n",data[0].value,op[0],data[1].value,op[1],data[2].value,op[2],data[3].value,rst.value/rst.amp);
								}
								//5. (A ? (B ? C)) ? D
								rst = calc(calc(data[0], calc(data[1], data[2], op[1]), op[0]), data[3], op[2]);
								if ((rst.value%rst.amp)==0 && rst.div0==0 && (rst.value/rst.amp)==24) {
									counter++;
									printf("(%d%c(%d%c%d))%c%d=%d\n",data[0].value,op[0],data[1].value,op[1],data[2].value,op[2],data[3].value,rst.value/rst.amp);
								}
								//计算完毕
							}
						}
					}
					//操作符分配完毕					
				}
			}
		}
	}	//排列完毕
	if(!counter) puts("no solutions :(");
		
}
