
//a node in the bplustree is represented as an offset
//the real node struct:xx_node
#include<cstdio>
#include<cstdlib>

#define MAX_KEY 5

typedef int Key;
typedef unsigned int Pointer;
typedef unsigned int Offset;//in the file

struct Key_Pointer
{
	Key key;
	Pointer pointer;
};

class Node
{
public:
	Key key[MAX_KEY];
	Pointer pointer[MAX_KEY + 1];
	int key_total;
	bool is_leaf;
	Node() {
		for (int i = 0; i<MAX_KEY; ++i)
		{
			key[i] = -1;
			pointer[i] = -1;// ??MAX_KEY is the pointer or xx.key_total
		}
		pointer[MAX_KEY] = -1;
		key_total = 0;
		is_leaf = false;
	}
	void copy(Node& dest) {
		for (int i = 0; i<MAX_KEY; ++i)
		{
			dest.key[i] = key[i];
			dest.pointer[i] = pointer[i];// ??MAX_KEY is the pointer or xx.key_total
		}
		dest.pointer[MAX_KEY] = pointer[MAX_KEY];
		dest.key_total = key_total;
		dest.is_leaf = is_leaf;
	}
	void print() {
		for (int i = 0; i<MAX_KEY; ++i)
		{
			printf("key[%d]=%d\t", i, key[i]);
			printf("pointer[%d]=%u\t", i, pointer[i]);
			printf("\n");
		}
		printf("pointer[%d]=%u\n", MAX_KEY, pointer[MAX_KEY]);

		printf("key_total=%d\n", key_total);
		printf("is_leaf=%d\n", is_leaf);
	}
};

void print_index(const char filename[], int size1)
{
	FILE* fp;
	fp = fopen("index", "rb");
	fseek(fp, 0, SEEK_END);
	int total_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	for (int pos = 0; pos<total_size; pos += size1)
	{
		fseek(fp, pos, SEEK_SET);
		Node n;
		fread(&n, sizeof(Node), 1, fp);
		printf("offset:%d\n", pos);
		n.print();
		printf("-----------\n");

	}

}
class Bp_tree
{
	Offset root;//the offset of the root in the file; default to be 0
	FILE* bp_file;
public:
	Bp_tree(const char filename[]);
	~Bp_tree();
	void build();
	void insert(const Key_Pointer& kp);
	Pointer query(Key key, bool get_bpos = false);
	void remove(Key key);
	void toggle();

private:
	int get_final_node();
	void write_node_to_file(const Offset bp_offset, const Node& n);
	void read_node_from_file(const Offset bp_offset, Node& n);
	void insert_pos(const Key_Pointer& kp, Offset curr);
	void split(Node& parent_node, Node& child_node, const int child_num);
	Pointer query_pos(Key key, Offset curr, bool get_bpos = false);
	void remove_pos(Key key, Offset curr);
};
void Bp_tree::toggle()
{
	fclose(bp_file);
	bp_file = fopen("index", "rb+");
}
Bp_tree::Bp_tree(const char filename[])
{
	bp_file = fopen(filename, "rb+");
	root = 0;
}
Bp_tree::~Bp_tree()
{
	fclose(bp_file);
}
void Bp_tree::build()
{
	int start = get_final_node();
	if (start != 0)
	{
		printf("Building from an existing file...\n");
		return;
	}
	printf("Building from an empty file...\n");
	Node n;
	n.pointer[MAX_KEY] = 0;
	n.key_total = 0;
	n.is_leaf = true;
	write_node_to_file(start, n);
}
int Bp_tree::get_final_node()
{
	fseek(bp_file, 0, SEEK_END);
	return ftell(bp_file);
}

void Bp_tree::write_node_to_file(const Offset bp_offset, const Node& n)
{
	fseek(bp_file, bp_offset, SEEK_SET);
	fwrite(&n, sizeof(Node), 1, bp_file);
}
void Bp_tree::read_node_from_file(const Offset bp_offset, Node& n)
{
	fseek(bp_file, bp_offset, SEEK_SET);
	fread(&n, sizeof(Node), 1, bp_file);
}
void Bp_tree::insert(const Key_Pointer& kp)
{
	//printf("Inserting a key-pointer pair...\n");
	Node root_node;
	read_node_from_file(root, root_node);
	if (root_node.key_total == MAX_KEY)
	{
		//printf("Spliting the root...\n");
		int last = get_final_node();
		Node new_root_node;
		new_root_node.is_leaf = false;
		new_root_node.key_total = 0;
		new_root_node.key[0] = root_node.key[root_node.key_total - 1];
		new_root_node.pointer[0] = root;//I suppose the final pointer to be null


		write_node_to_file(last, new_root_node);

		split(new_root_node, root_node, 0);
		Node tmp_node;
		new_root_node.copy(tmp_node);

		tmp_node.pointer[0] = last;
		write_node_to_file(last, root_node);
		write_node_to_file(0, tmp_node);

		root = 0;
	}
	insert_pos(kp, root);
}
void Bp_tree::insert_pos(const Key_Pointer& kp, Offset curr)
{
	//printf("\tInserting a key-pointer to a pos...\n");
	//fseek(bp_file,curr,SEEK_SET);
	Node curr_node;
	read_node_from_file(curr, curr_node);
	int i;
	for (i = 0; i<curr_node.key_total&&curr_node.key[i]<kp.key; ++i);
	//!! i<curr_node.key_total是需要的！！不然可能刚好就后面一个key和我们的key相等
	if (i<curr_node.key_total&&curr_node.key[i] == kp.key)//already exists
		return;


	if (curr_node.is_leaf == false)//not a leaf
	{
		Node child_node;
		read_node_from_file(curr_node.pointer[i], child_node);
		if (child_node.key_total == MAX_KEY)
		{
			if (kp.key == 8)
			{
				int a = 1;
			}
			//split(curr,i); i is not the offset of the curr
			if (i == 1)
			{
				i = 1;
			}
			split(curr_node, child_node, i);

			write_node_to_file(curr, curr_node);

			write_node_to_file(curr_node.pointer[i], child_node);


		}

		if (curr_node.key[i] >= kp.key || i == curr_node.key_total)
		{
			insert_pos(kp, curr_node.pointer[i]);
		}
		else
		{
			insert_pos(kp, curr_node.pointer[i + 1]);
		}
	}
	else
	{
		//int i;
		//for(i=0;i<curr_node.key_total&&curr_node.key[i]<kp.key;++i);
		//if(curr_node.key[i]==kp.key)
		//  return;
		int inserted_pos = i;
		for (int j = curr_node.key_total; j > inserted_pos; --j)
		{
			curr_node.key[j] = curr_node.key[j - 1];
			curr_node.pointer[j] = curr_node.pointer[j - 1];
		}

		curr_node.key[inserted_pos] = kp.key;
		curr_node.pointer[inserted_pos] = kp.pointer;
		curr_node.key_total++;
		write_node_to_file(curr, curr_node);
	}

}
void Bp_tree::split(Node& parent_node, Node& child_node, const int child_num)
{
	//printf("Splitting...\n");

	int half = MAX_KEY >> 1;
	parent_node.pointer[parent_node.key_total + 1] = parent_node.pointer[parent_node.key_total];
	for (int j = parent_node.key_total; j>child_num; --j)
	{
		parent_node.key[j] = parent_node.key[j - 1];
		parent_node.pointer[j] = parent_node.pointer[j - 1];
	}
	parent_node.key[child_num] = child_node.key[half];
	parent_node.key_total++;

	int right = get_final_node();
	Node right_node;
	right_node.key_total = MAX_KEY - half - 1;
	for (int i = half + 1; i<MAX_KEY; ++i)
	{
		right_node.key[i - half - 1] = child_node.key[i];
		right_node.pointer[i - half - 1] = child_node.pointer[i];
	}
	right_node.is_leaf = child_node.is_leaf;

	//right_node.key[right_node.key_total] = child_node.key[MAX_KEY];
	right_node.pointer[right_node.key_total] = child_node.pointer[MAX_KEY];


	parent_node.pointer[child_num + 1] = right;


	child_node.key_total = half;
	if (child_node.is_leaf == true)
	{
		child_node.key_total++;

		right_node.pointer[MAX_KEY] = child_node.pointer[MAX_KEY];
		child_node.pointer[MAX_KEY] = right;
	}

	write_node_to_file(right, right_node);
}

// if not find, return -1
//if get_bpos is true,then we find the pos of leaf in the bp_file(0,52,..)
Pointer Bp_tree::query(Key key, bool get_bpos)
{
	return  query_pos(key, root, get_bpos);
}

Pointer Bp_tree::query_pos(Key key, Offset curr, bool get_bpos)
{
	Node curr_node;
	read_node_from_file(curr, curr_node);


	//
	//有可能是删除后还没有处理的一个状态，就是说根节点下面还没有赋值上来（本来应该再次删除时才可以的）
	if (curr_node.key_total == 0 && curr_node.is_leaf == false)
	{
		return query_pos(key, curr_node.pointer[0], get_bpos);
	}

	if (!curr_node.is_leaf)
	{
		int i;
		for (i = 0; i < curr_node.key_total&&curr_node.key[i] < key; ++i);
		return query_pos(key, curr_node.pointer[i], get_bpos);
	}
	else
	{
		int i;
		for (i = 0; i < curr_node.key_total&&curr_node.key[i] < key; ++i);

		//!! i < curr_node.key_total
		if (i<curr_node.key_total&&curr_node.key[i] == key)
			return get_bpos ? curr : curr_node.pointer[i];
		else
			return -1;
	}
}

void Bp_tree::remove(Key key)
{
	remove_pos(key, root);

}

void Bp_tree::remove_pos(Key key, Offset curr)
{
	int i, j;

	Node curr_node;
	read_node_from_file(curr, curr_node);

	//
	//当子节点只有一个的时候，就要向上走。
	//并且此时该节点只可能是根节点。假设该节点是内节点，则其key_total为0，则在其为1之前，如果有兄弟，就和兄弟合并了。
	//如果没有兄弟，则我们应该先在该节点的父节点就处理了（相同情况更先遇到），所以只可能是该节点为根节点
	if (curr_node.key_total == 0)
	{
		Node child_node;
		read_node_from_file(curr_node.pointer[0], child_node);
		child_node.copy(curr_node);
		write_node_to_file(curr, curr_node);
		remove_pos(key, curr);
		return;

	}


	for (i = 0; i < curr_node.key_total && key > curr_node.key[i]; i++);

	if (i < curr_node.key_total && curr_node.key[i] == key)  //在当前节点找到关键字  
	{

		if (!curr_node.is_leaf)     //在内节点找到关键字  
		{
			Node child_node;
			read_node_from_file(curr_node.pointer[i], child_node);

			if (child_node.is_leaf)     //如果孩子是叶节点  
			{
				if (child_node.key_total > MAX_KEY / 2)      //情况A  
				{//只要把孩子的最后一个给删掉就可以了
					curr_node.key[i] = child_node.key[child_node.key_total - 2];
					child_node.key_total--;

					write_node_to_file(curr, curr_node);
					write_node_to_file(curr_node.pointer[i], child_node);

					return;
				}
				else    //否则孩子节点的关键字数量不过半  
				{
					if (i > 0)      //有左兄弟节点  
					{
						Node lbchild_node;
						read_node_from_file(curr_node.pointer[i - 1], lbchild_node);

						if (lbchild_node.key_total > MAX_KEY / 2)        //情况B  
						{
							for (j = child_node.key_total; j > 0; j--)//child_node整体右移一个单位，空出0位置
							{
								child_node.key[j] = child_node.key[j - 1];
								child_node.pointer[j] = child_node.pointer[j - 1];
							}

							child_node.key[0] = curr_node.key[i - 1];//child_node的0位置放左边兄弟的最后一个key-pointer
							child_node.pointer[0] = lbchild_node.pointer[lbchild_node.key_total - 1];

							child_node.key_total++;//?? 我感觉这里不用变吧

							lbchild_node.key_total--;

							//更新父节点
							curr_node.key[i - 1] = lbchild_node.key[lbchild_node.key_total - 1];//左边有新的最大值
							curr_node.key[i] = child_node.key[child_node.key_total - 2];//右边也有新的最大值

							write_node_to_file(curr, curr_node);
							write_node_to_file(curr_node.pointer[i - 1], lbchild_node);
							write_node_to_file(curr_node.pointer[i], child_node);

						}
						else    //情况C  左边兄弟不够借
						{
							//lbchild starts
							for (j = 0; j < child_node.key_total; j++)//右边节点的key_pointer放到左边节点里面
							{
								lbchild_node.key[lbchild_node.key_total + j] = child_node.key[j];
								lbchild_node.pointer[lbchild_node.key_total + j] = child_node.pointer[j];
							}
							lbchild_node.key_total += child_node.key_total;

							lbchild_node.pointer[MAX_KEY] = child_node.pointer[MAX_KEY];
							//lbchild ends
							//释放child节点占用的空间x.pointer[i]  

							// p0 k0,p1 k1,p2 k2,p3
							// suppose i:1
							// => p0 k1,p2 k2,p3	(k1 is deleted)
							for (j = i - 1; j < curr_node.key_total - 1; j++)
							{
								curr_node.key[j] = curr_node.key[j + 1];
								curr_node.pointer[j + 1] = curr_node.pointer[j + 2];
							}
							curr_node.key_total--;

							//k1 is deleted, so use the new k1.此时整体的key_total还没有-1,so "-2" here
							//当我们处理叶节点的时候，才会减去这个1
							curr_node.key[i - 1] = lbchild_node.key[lbchild_node.key_total - 2];

							write_node_to_file(curr, curr_node);
							write_node_to_file(curr_node.pointer[i - 1], lbchild_node);

							i--;

						}


					}
					else      //只有右兄弟节点  
					{
						Node rbchild_node;
						read_node_from_file(curr_node.pointer[i + 1], rbchild_node);

						if (rbchild_node.key_total > MAX_KEY / 2)        //情况D  右边的兄弟过半满，借一个节点
						{
							//curr_node starts
							curr_node.key[i] = rbchild_node.key[0];
							//curr_node ends

							//child_node starts
							child_node.key[child_node.key_total] = rbchild_node.key[0];
							child_node.pointer[child_node.key_total] = rbchild_node.pointer[0];
							child_node.key_total++;
							//child_node starts

							//rbchild_node starts
							for (j = 0; j < rbchild_node.key_total - 1; j++)
							{
								rbchild_node.key[j] = rbchild_node.key[j + 1];
								rbchild_node.pointer[j] = rbchild_node.pointer[j + 1];
							}
							rbchild_node.key_total--;
							//rbchild_node ends

							write_node_to_file(curr, curr_node);
							write_node_to_file(curr_node.pointer[i], child_node);
							write_node_to_file(curr_node.pointer[i + 1], rbchild_node);

						}
						else    //情况E  右边的兄弟不够，只能合并了
						{
							//child_node starts
							for (j = 0; j < rbchild_node.key_total; j++)
							{
								child_node.key[child_node.key_total + j] = rbchild_node.key[j];
								child_node.pointer[child_node.key_total + j] = rbchild_node.pointer[j];
							}
							child_node.key_total += rbchild_node.key_total;

							child_node.pointer[MAX_KEY] = rbchild_node.pointer[MAX_KEY];
							//child_node ends

							//释放rbchild占用的空间x.Pointer[i+1]  

							//curr_node starts
							for (j = i; j < curr_node.key_total - 1; j++)
							{
								curr_node.key[j] = curr_node.key[j + 1];
								curr_node.pointer[j + 1] = curr_node.pointer[j + 2];
							}
							curr_node.key_total--;
							//curr_node ends


							write_node_to_file(curr, curr_node);
							write_node_to_file(curr_node.pointer[i], child_node);

						}

					}

				}

			}
			else      //情况F  孩子是内节点
			{

				//找到key在B+树叶节点的左兄弟关键字,将这个关键字取代key的位置  

				/*TRecord trecord;
				trecord.key = record.key;
				SearchResult result;
				Search_BPlus_Tree(trecord, result);*/
				Pointer result = query(key, true);


				Node last_node;

				read_node_from_file(result, last_node);

				curr_node.key[i] = last_node.key[last_node.key_total - 2];//把叶节点的倒数第一个key_pointer扔到我的位置(最后一个删掉了)

				write_node_to_file(curr, curr_node);


				if (child_node.key_total > MAX_KEY / 2)        //情况H  子节点过半满
				{

				}
				else          //否则孩子节点的关键字数量不过半,则将兄弟节点的某一个关键字移至孩子  
				{
					if (i > 0)  //x.key[i]有左兄弟  
					{
						Node lbchild_node;
						read_node_from_file(curr_node.pointer[i - 1], lbchild_node);

						if (lbchild_node.key_total > MAX_KEY / 2)       //情况I  左边的兄弟过半满，借一个
						{
							//child_node starts
							for (j = child_node.key_total; j > 0; j--)
							{
								child_node.key[j] = child_node.key[j - 1];
								child_node.pointer[j + 1] = child_node.pointer[j];
							}
							child_node.pointer[1] = child_node.pointer[0];
							child_node.key[0] = curr_node.key[i - 1];
							child_node.pointer[0] = lbchild_node.pointer[lbchild_node.key_total];

							child_node.key_total++;
							//child_node ends

							//curr_node starts
							curr_node.key[i - 1] = lbchild_node.key[lbchild_node.key_total - 1];
							//curr_node ends

							//lbchild_node starts
							lbchild_node.key_total--;
							//lbchild_node ends

							write_node_to_file(curr, curr_node);
							write_node_to_file(curr_node.pointer[i - 1], lbchild_node);
							write_node_to_file(curr_node.pointer[i], child_node);
						}
						else        //情况J  左边的兄弟不够，要合并了
						{
							//lbchild_node starts
							//??下面一行多余了吧 后面for循环种已经又赋值了
							//!!不用，因为下面key_total++了						
							lbchild_node.key[lbchild_node.key_total] = curr_node.key[i - 1];   //将孩子节点复制到其左兄弟的末尾  
							lbchild_node.key_total++;

							for (j = 0; j < child_node.key_total; j++)      //将child节点拷贝到lbchild节点的末尾,  
							{
								lbchild_node.key[lbchild_node.key_total + j] = child_node.key[j];
								lbchild_node.pointer[lbchild_node.key_total + j] = child_node.pointer[j];//??lbchild的pointer[key_total]是不是被覆盖了
							}
							lbchild_node.pointer[lbchild_node.key_total + j] = child_node.pointer[j];
							lbchild_node.key_total += child_node.key_total;        //已经将child拷贝到lbchild节点  
																				   //lbchild_node ends


																				   //释放child节点的存储空间,curr_node.Pointer[i]  


																				   //curr_node starts
																				   //将找到关键字的孩子child与关键字左兄弟的孩子lbchild合并后,将该关键字前移,使当前节点的关键字减少一个  
							for (j = i - 1; j < curr_node.key_total - 1; j++)
							{
								curr_node.key[j] = curr_node.key[j + 1];
								curr_node.pointer[j + 1] = curr_node.pointer[j + 2];
							}
							curr_node.key_total--;
							//curr_node ends


							write_node_to_file(curr, curr_node);
							write_node_to_file(curr_node.pointer[i - 1], lbchild_node);

							i--;

						}

					}
					else        //否则x.key[i]只有右兄弟  
					{
						Node rbchild_node;
						read_node_from_file(curr_node.pointer[i + 1], rbchild_node);

						if (rbchild_node.key_total > MAX_KEY / 2)     //情况K  右兄弟过半满，借一个
						{

							//child_node starts 借一个叶节点过来，更新下自己的key
							child_node.key[child_node.key_total] = curr_node.key[i];
							child_node.key_total++;
							child_node.pointer[child_node.key_total] = rbchild_node.pointer[0];
							//child_node starts


							//curr_node starts 
							curr_node.key[i] = rbchild_node.key[0];
							//curr_node ends

							//rbchild_node starts　左移一项
							for (j = 0; j < rbchild_node.key_total - 1; j++)
							{
								rbchild_node.key[j] = rbchild_node.key[j + 1];
								rbchild_node.pointer[j] = rbchild_node.pointer[j + 1];
							}
							rbchild_node.pointer[j] = rbchild_node.pointer[j + 1];
							rbchild_node.key_total--;
							//rbchild_node ends

							write_node_to_file(curr, curr_node);
							write_node_to_file(curr_node.pointer[i], child_node);
							write_node_to_file(curr_node.pointer[i + 1], rbchild_node);

						}
						else        //情况L  右兄弟不够，合并
						{
							//child_node starts
							//??下面那个应该从j=1开始遍历total次吧
							//!!不用！，因为key_total++了
							child_node.key[child_node.key_total] = curr_node.key[i];
							child_node.key_total++;

							for (j = 0; j < rbchild_node.key_total; j++)     //将rbchild节点合并到child节点后  
							{
								child_node.key[child_node.key_total + j] = rbchild_node.key[j];
								child_node.pointer[child_node.key_total + j] = rbchild_node.pointer[j];
							}
							child_node.pointer[child_node.key_total + j] = rbchild_node.pointer[j];

							child_node.key_total += rbchild_node.key_total;
							//child_node ends


							//释放rbchild节点所占用的空间,x,Pointer[i+1]  

							//curr_node starts 前移一个
							for (j = i; j < curr_node.key_total - 1; j++)    //当前将关键字之后的关键字左移一位,使该节点的关键字数量减一  
							{
								curr_node.key[j] = curr_node.key[j + 1];
								curr_node.pointer[j + 1] = curr_node.pointer[j + 2];
							}
							curr_node.key_total--;
							//curr_node ends

							write_node_to_file(curr, curr_node);
							write_node_to_file(curr_node.pointer[i], child_node);

						}

					}
				}

			}

			remove_pos(key, curr_node.pointer[i]);

		}
		else  //情况G  在curr_node中找到了key，curr_node为叶节点
		{
			//curr_node starts
			for (j = i; j < curr_node.key_total - 1; j++)
			{
				curr_node.key[j] = curr_node.key[j + 1];
				curr_node.pointer[j] = curr_node.pointer[j + 1];
			}
			curr_node.key_total--;
			//curr_node starts

			write_node_to_file(curr, curr_node);

			return;
		}

	}
	else        //在当前节点没找到关键字     
	{
		if (!curr_node.is_leaf)    //没找到关键字,则关键字必然包含在以Pointer[i]为根的子树中  
		{
			Node child_node;
			read_node_from_file(curr_node.pointer[i], child_node);

			if (!child_node.is_leaf)      //如果其孩子节点是内节点  
			{
				if (child_node.key_total > MAX_KEY / 2)        //情况H  
				{

				}
				else          //否则孩子节点的关键字数量不过半,则将兄弟节点的某一个关键字移至孩子  
				{
					if (i > 0)  //x.key[i]有左兄弟  
					{
						Node lbchild_node;
						read_node_from_file(curr_node.pointer[i - 1], lbchild_node);

						if (lbchild_node.key_total > MAX_KEY / 2)       //情况I  左节点过半满了，借一个呗
						{

							//child_node starts
							//子节点右移一位，从左兄弟那里拿左兄弟的最后一个放在第一个位置，其中用父节点的key来过渡一下
							for (j = child_node.key_total; j > 0; j--)
							{
								child_node.key[j] = child_node.key[j - 1];
								child_node.pointer[j + 1] = child_node.pointer[j];
							}
							child_node.pointer[1] = child_node.pointer[0];
							child_node.key[0] = curr_node.key[i - 1];
							child_node.pointer[0] = lbchild_node.pointer[lbchild_node.key_total];

							child_node.key_total++;
							//child_node ends

							//curr_node starts
							//父节点只要更新一下key[i-1]就好了
							curr_node.key[i - 1] = lbchild_node.key[lbchild_node.key_total - 1];
							//curr_node ends

							//lbchild_node starts
							//左节点只要总的key数量-1就好了
							lbchild_node.key_total--;
							//lbchild_node ends


							write_node_to_file(curr, curr_node);
							write_node_to_file(curr_node.pointer[i - 1], lbchild_node);
							write_node_to_file(curr_node.pointer[i], child_node);
						}
						else        //情况J  左节点不够，在一起吧
						{
							//lbchild_node starts
							//左兄弟和子节点连接在一起，中间用父节点的key[i-1]过渡,pointer[key_total]也需要
							lbchild_node.key[lbchild_node.key_total] = curr_node.key[i - 1];   //将孩子节点复制到其左兄弟的末尾  
							lbchild_node.key_total++;

							for (j = 0; j < child_node.key_total; j++)      //将child节点拷贝到lbchild节点的末尾,  
							{
								lbchild_node.key[lbchild_node.key_total + j] = child_node.key[j];
								lbchild_node.pointer[lbchild_node.key_total + j] = child_node.pointer[j];
							}
							lbchild_node.pointer[lbchild_node.key_total + j] = child_node.pointer[j];
							lbchild_node.key_total += child_node.key_total;        //已经将child拷贝到lbchild节点  
																				   //lbchild_node ends


																				   //释放child节点的存储空间,x.Pointer[i]  


																				   //curr_node starts
																				   //将找到关键字的孩子child与关键字左兄弟的孩子lbchild合并后,将该关键字前移,使当前节点的关键字减少一个  
							for (j = i - 1; j < curr_node.key_total - 1; j++)
							{
								curr_node.key[j] = curr_node.key[j + 1];
								curr_node.pointer[j + 1] = curr_node.pointer[j + 2];
							}
							curr_node.key_total--;
							//curr_node ends

							write_node_to_file(curr, curr_node);
							write_node_to_file(curr_node.pointer[i - 1], lbchild_node);

							i--;

						}

					}
					else        //否则x.key[i]只有右兄弟  
					{
						Node rbchild_node;
						read_node_from_file(curr_node.pointer[i + 1], rbchild_node);

						if (rbchild_node.key_total > MAX_KEY / 2)     //情况K  有兄弟过半满了，借一个呗
						{

							//child_node starts
							//子节点的最后一个放右兄弟的第一个，中间要有父节点的key[i]过渡
							child_node.key[child_node.key_total] = curr_node.key[i];
							child_node.key_total++;
							child_node.pointer[child_node.key_total] = rbchild_node.pointer[0];
							//child_node ends

							//curr_node starts
							//父节点更新一下key[i]就行了
							curr_node.key[i] = rbchild_node.key[0];
							//curr_node ends


							//rbchild_node starts
							for (j = 0; j < rbchild_node.key_total - 1; j++)
							{
								rbchild_node.key[j] = rbchild_node.key[j + 1];
								rbchild_node.pointer[j] = rbchild_node.pointer[j + 1];
							}
							rbchild_node.pointer[j] = rbchild_node.pointer[j + 1];
							rbchild_node.key_total--;
							//rbchild_node ends

							write_node_to_file(curr, curr_node);
							write_node_to_file(curr_node.pointer[i], child_node);
							write_node_to_file(curr_node.pointer[i + 1], rbchild_node);

						}
						else        //情况L  右兄弟不够用，在一起吧
						{
							//child_node starts
							//内节点合并时需要借助父节点的key过渡
							//子节点用父节点的key过渡，和右兄弟拼接 pointer[key_total]是需要的
							child_node.key[child_node.key_total] = curr_node.key[i];
							child_node.key_total++;

							for (j = 0; j < rbchild_node.key_total; j++)     //将rbchild节点合并到child节点后  
							{
								child_node.key[child_node.key_total + j] = rbchild_node.key[j];
								child_node.pointer[child_node.key_total + j] = rbchild_node.pointer[j];
							}
							child_node.pointer[child_node.key_total + j] = rbchild_node.pointer[j];

							child_node.key_total += rbchild_node.key_total;
							//child_node ends

							//释放rbchild节点所占用的空间,x,Pointer[i+1]  

							//curr_node starts
							//父节点左移一项
							for (j = i; j < curr_node.key_total - 1; j++)    //当前将关键字之后的关键字左移一位,使该节点的关键字数量减一  
							{
								curr_node.key[j] = curr_node.key[j + 1];
								curr_node.pointer[j + 1] = curr_node.pointer[j + 2];
							}
							curr_node.key_total--;
							//curr_node ends

							write_node_to_file(curr, curr_node);
							write_node_to_file(curr_node.pointer[i], child_node);

						}

					}
				}
			}
			else  //否则其孩子节点是外节点(叶节点)
			{
				if (child_node.key_total > MAX_KEY / 2)  //情况M  子节点状态很好，不用处理
				{

				}
				else        //否则孩子节点不到半满  
				{
					if (i > 0) //有左兄弟  
					{
						Node lbchild_node;
						read_node_from_file(curr_node.pointer[i - 1], lbchild_node);

						if (lbchild_node.key_total > MAX_KEY / 2)       //情况N  左兄弟过半满了，借一个
						{
							//child_node starts 子节点右移一项，把左兄弟的最后一项拿过来
							for (j = child_node.key_total; j > 0; j--)
							{
								child_node.key[j] = child_node.key[j - 1];
								child_node.pointer[j] = child_node.pointer[j - 1];
							}
							child_node.key[0] = curr_node.key[i - 1];
							child_node.pointer[0] = lbchild_node.pointer[lbchild_node.key_total - 1];
							child_node.key_total++;
							//child_node ends

							//lbchild_node starts 左兄弟丢弃最后一项
							lbchild_node.key_total--;
							//lbchild_node ends

							//curr_node starts 父节点只要更新一下自己的一个key就ok
							curr_node.key[i - 1] = lbchild_node.key[lbchild_node.key_total - 1];
							//curr_node ends

							write_node_to_file(curr_node.pointer[i - 1], lbchild_node);
							write_node_to_file(curr_node.pointer[i], child_node);
							write_node_to_file(curr, curr_node);

						}
						else        //情况O	左兄弟不够了，合并,
						{
							//lbchild_node starts
							//??是不是要把父节点的拿过来
							//!!不用，因为两个都是叶节点
							//左兄弟把右节点的所有东西(包括指向下一项的指针)都拿过来，<u>首尾需要父节点的key过渡</u>
							for (j = 0; j < child_node.key_total; j++)        //与左兄弟孩子节点合并  
							{
								lbchild_node.key[lbchild_node.key_total + j] = child_node.key[j];
								lbchild_node.pointer[lbchild_node.key_total + j] = child_node.pointer[j];
							}
							lbchild_node.key_total += child_node.key_total;

							lbchild_node.pointer[MAX_KEY] = child_node.pointer[MAX_KEY];
							//lbchild_node ends

							//释放child占用的空间x.Pointer[i]  

							//curr_node starts
							//父节点往左移一项
							for (j = i - 1; j < curr_node.key_total - 1; j++)
							{
								curr_node.key[j] = curr_node.key[j + 1];
								curr_node.pointer[j + 1] = curr_node.pointer[j + 2];
							}

							curr_node.key_total--;
							//curr_node ends

							write_node_to_file(curr_node.pointer[i - 1], lbchild_node);
							write_node_to_file(curr, curr_node);

							i--;

						}

					}
					else        //否则只有右兄弟  
					{
						Node rbchild_node;
						read_node_from_file(curr_node.pointer[i + 1], rbchild_node);

						if (rbchild_node.key_total > MAX_KEY / 2)       //情况P  右兄弟过半满了，借一个
						{
							//curr_node starts
							// 父节点只要更新key就好了
							curr_node.key[i] = rbchild_node.key[0];
							//curr_node ends

							//child_node starts
							// 子节点只要接受右边节点的一个项就好了
							child_node.key[child_node.key_total] = rbchild_node.key[0];
							child_node.pointer[child_node.key_total] = rbchild_node.pointer[0];
							child_node.key_total++;
							//child_node ends

							//rbchild_node starts
							// 右节点左移一项
							for (j = 0; j < rbchild_node.key_total - 1; j++)
							{
								rbchild_node.key[j] = rbchild_node.key[j + 1];
								rbchild_node.pointer[j] = rbchild_node.pointer[j + 1];
							}
							rbchild_node.key_total--;
							//rbchild_node ends

							write_node_to_file(curr, curr_node);
							write_node_to_file(curr_node.pointer[i + 1], rbchild_node);
							write_node_to_file(curr_node.pointer[i], child_node);

						}
						else        //情况Q  右节点不够了，在一起吧
						{
							//child_node starts
							//子节点把右节点全部接受，后面的MAX_KEY的指针也要
							for (j = 0; j < rbchild_node.key_total; j++)
							{
								child_node.key[child_node.key_total + j] = rbchild_node.key[j];
								child_node.pointer[child_node.key_total + j] = rbchild_node.pointer[j];
							}
							child_node.key_total += rbchild_node.key_total;
							child_node.pointer[MAX_KEY] = rbchild_node.pointer[MAX_KEY];
							//child_node ends

							//释放rbchild占用的空间x.Pointer[i+1]  

							//curr_node starts
							//父节点向左移一项，并更新key[i](也可以直接用key[i+1]赋值)
							for (j = i; j < curr_node.key_total - 1; j++)
							{
								curr_node.key[j] = curr_node.key[j + 1];
								curr_node.pointer[j + 1] = curr_node.pointer[j + 2];
							}
							curr_node.key_total--;
							//curr_node ends

							write_node_to_file(curr, curr_node);
							write_node_to_file(curr_node.pointer[i], child_node);


						}

					}

				}

			}

			remove_pos(key, curr_node.pointer[i]);
		}
		//如果该节点为叶节点，那么在该节点内没找到关键字，就说明b+树中没有该关键字


	}


}


int main()
{
	//create a new file
	remove("index");
	FILE* tmp = fopen("index", "w");
	fclose(tmp);

	const bool debug = false;
	{
		Bp_tree bt("index");
		bt.build();
		Key_Pointer kp;
		const int total = 500;
		//for (int i = total; i >= 0; --i)
		for (int i = total - 1; i >= 0; --i)
		{
			kp.key = i;
			kp.pointer = i * 100;
			bt.insert(kp);
		}

		if (debug)
		{
			bt.toggle();
			print_index("index", sizeof(Node));

		}

		for (int i = 0; i<total; ++i)
			printf("%d:data:%d\tb_pos:%d\n", i, bt.query(i), bt.query(i, true));
		printf("***************************\n");

		//删除
		for (int i = 0; i < total; ++i)
		{
			//int d = rand() % total;;
			int d = i;

			//??保留三个的话就全部都被删除掉了
			if (d % 3 == 0)
				continue;

			if (debug)
				printf("deleting %d...\n-----------\n", d);
			bt.remove(d);


			if (debug)
			{
				bt.toggle();
				printf("*****************\n");
				print_index("index", sizeof(Node));
				printf("*****************\n");
			}

		}

		bt.toggle();
		for (int i = 0; i<total; ++i)
			printf("%d:data:%d\tb_pos:%d\n", i, bt.query(i), bt.query(i, true));

		//再次插入
		for (int i = total - 1; i >= 0; --i)
		{
			printf("\tInserting %d...\n", i);
			kp.key = i;
			kp.pointer = i * 100;
			bt.insert(kp);

			if (debug)
			{
				bt.toggle();
				print_index("index", sizeof(Node));

			}
		}


		bt.toggle();
		for (int i = 0; i<total; ++i)
			printf("%d:data:%d\tb_pos:%d\n", i, bt.query(i), bt.query(i, true));
	}


	if (debug)
	{
		print_index("index", sizeof(Node));
	}

}