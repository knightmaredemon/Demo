#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <cmath>
using namespace std;




struct KdTree{
    vector<double> root;
    KdTree* parent;
    KdTree* leftChild;
    KdTree* rightChild;
    //Ĭ�Ϲ��캯��
    KdTree(){parent = leftChild = rightChild = NULL;}
    //�ж�kd���Ƿ�Ϊ��
    bool isEmpty()
    {
        return root.empty();
    }
    //�ж�kd���Ƿ�ֻ��һ��Ҷ�ӽ��
    bool isLeaf()
    {
        return (!root.empty()) && 
            rightChild == NULL && leftChild == NULL;
    }
    //�ж��Ƿ������ĸ����
    bool isRoot()
    {
        return (!isEmpty()) && parent == NULL;
    }
    //�жϸ���kd���ĸ�����Ƿ����丸kd��������
    bool isLeft()
    {
        return parent->leftChild->root == root;
    }
    //�жϸ���kd���ĸ�����Ƿ����丸kd�����ҽ��
    bool isRight()
    {
        return parent->rightChild->root == root;
    }
};

int data[6][2] = {{2,3},{5,4},{9,6},{4,7},{8,1},{7,2}};

template<typename T>
vector<vector<T> > Transpose(vector<vector<T> > Matrix)
{
    unsigned row = Matrix.size();
    unsigned col = Matrix[0].size();
    vector<vector<T> > Trans(col,vector<T>(row,0));
    for (unsigned i = 0; i < col; ++i)
    {
        for (unsigned j = 0; j < row; ++j)
        {
            Trans[i][j] = Matrix[j][i];
        }
    }
    return Trans;
}

template <typename T>
T findMiddleValue(vector<T> vec)
{
    sort(vec.begin(),vec.end());
    auto pos = vec.size() / 2;
    return vec[pos];
}


//����kd��
void buildKdTree(KdTree* tree, vector<vector<double> > data, unsigned depth)
{

    //����������
    unsigned samplesNum = data.size();
    //��ֹ����
    if (samplesNum == 0)
    {
        return;
    }
    if (samplesNum == 1)
    {
        tree->root = data[0];
        return;
    }
    //������ά��
    unsigned k = data[0].size();
    vector<vector<double> > transData = Transpose(data);
    //ѡ���з�����
    unsigned splitAttribute = depth % k;
    vector<double> splitAttributeValues = transData[splitAttribute];
    //ѡ���з�ֵ
    double splitValue = findMiddleValue(splitAttributeValues);
    //cout << "splitValue" << splitValue  << endl;

    // ����ѡ�����з����Ժ��з�ֵ�������ݼ���Ϊ�����Ӽ�
    vector<vector<double> > subset1;
    vector<vector<double> > subset2;
    for (unsigned i = 0; i < samplesNum; ++i)
    {
        if (splitAttributeValues[i] == splitValue && tree->root.empty())
            tree->root = data[i];
        else
        {
            if (splitAttributeValues[i] < splitValue)
                subset1.push_back(data[i]);
            else
                subset2.push_back(data[i]);
        }
    }

    //�Ӽ��ݹ����buildKdTree����

    tree->leftChild = new KdTree;
    tree->leftChild->parent = tree;
    tree->rightChild = new KdTree;
    tree->rightChild->parent = tree;
    buildKdTree(tree->leftChild, subset1, depth + 1);
    buildKdTree(tree->rightChild, subset2, depth + 1);
}

//����ӡkd��
void printKdTree(KdTree *tree, unsigned depth)
{
    for (unsigned i = 0; i < depth; ++i)
        cout << "\t";
            
    for (vector<double>::size_type j = 0; j < tree->root.size(); ++j)
        cout << tree->root[j] << ",";
    cout << endl;
    if (tree->leftChild == NULL && tree->rightChild == NULL )//Ҷ�ӽڵ�
        return;
    else //��Ҷ�ӽڵ�
    {
        if (tree->leftChild != NULL)
        {
            for (unsigned i = 0; i < depth + 1; ++i)
                cout << "\t";
            cout << " left:";
            printKdTree(tree->leftChild, depth + 1);
        }
            
        cout << endl;
        if (tree->rightChild != NULL)
        {
            for (unsigned i = 0; i < depth + 1; ++i)
                cout << "\t";
            cout << "right:";
            printKdTree(tree->rightChild, depth + 1);
        }
        cout << endl;
    }
}


//����ռ���������ľ���
double measureDistance(vector<double> point1, vector<double> point2, unsigned method)
{
    if (point1.size() != point2.size())
    {
        cerr << "Dimensions don't match����" ;
        exit(1);
    }
    switch (method)
    {
        case 0://ŷ�Ͼ���
            {
                double res = 0;
                for (vector<double>::size_type i = 0; i < point1.size(); ++i)
                {
                    res += pow((point1[i] - point2[i]), 2);
                }
                return sqrt(res);
            }
        case 1://�����پ���
            {
                double res = 0;
                for (vector<double>::size_type i = 0; i < point1.size(); ++i)
                {
                    res += abs(point1[i] - point2[i]);
                }
                return res;
            }
        default:
            {
                cerr << "Invalid method!!" << endl;
                return -1;
            }
    }
}
//��kd��tree������Ŀ���goal�������
//���룺Ŀ��㣻�ѹ����kd��
//�����Ŀ���������
vector<double> searchNearestNeighbor(vector<double> goal, KdTree *tree)
{
    /*��һ������kd�����ҳ�����Ŀ����Ҷ�ӽ�㣺�Ӹ���������
    �ݹ�����·���kd������Ŀ���ĵ�ǰά������С���зֵ��
    ���꣬���ƶ������ӽ�㣬�����ƶ������ӽ�㣬ֱ���ӽ��Ϊ
    Ҷ���Ϊֹ,�Դ�Ҷ�ӽ��Ϊ����ǰ����㡱
    */
    unsigned k = tree->root.size();//��������ݵ�ά��
    unsigned d = 0;//ά�ȳ�ʼ��Ϊ0�����ӵ�1ά��ʼ
    KdTree* currentTree = tree;
    vector<double> currentNearest = currentTree->root;
    while(!currentTree->isLeaf())
    {
        unsigned index = d % k;//���㵱ǰά
        if (currentTree->rightChild->isEmpty() || goal[index] < currentNearest[index])
        {
            currentTree = currentTree->leftChild;
        }
        else
        {
            currentTree = currentTree->rightChild;
        }
        ++d;
    }
    currentNearest = currentTree->root;

    /*�ڶ������ݹ�����ϻ��ˣ� ��ÿ�����������²�����
    (a)����ý�㱣���ʵ���ȵ�ǰ��������Ŀ�����������Ը�����Ϊ����ǰ����㡱
    (b)��ǰ�����һ��������ĳ���һ���ӽ���Ӧ�����򣬼����ӽ��ĸ�������
    һ�ӽ���Ӧ�����Ƿ��и����ĵ㣨�������һ�ӽ���Ӧ�������Ƿ�����Ŀ���Ϊ��
    �ġ���Ŀ����롰��ǰ����㡱��ľ���Ϊ�뾶�������ཻ��������ཻ����������һ
    ���ӽ���Ӧ�������ڴ��ھ�Ŀ�������ĵ㣬�ƶ�����һ���ӽ�㣬���ŵݹ������
    ����������������ཻ�����ϻ���*/

    //��ǰ�������Ŀ���ľ���
    double currentDistance = measureDistance(goal, currentNearest, 0);

    //�����ǰ��kd���ĸ�������丸�������ӣ��������丸�����Һ��ӽ��������
    //�����򣬷�֮�෴
    KdTree* searchDistrict;
    if (currentTree->isLeft())
    {
        if (currentTree->parent->rightChild == NULL)
            searchDistrict = currentTree;
        else
            searchDistrict = currentTree->parent->rightChild;
    }
    else
    {
        searchDistrict = currentTree->parent->leftChild;
    }

    //������������Ӧ����kd���ĸ���㲻������kd���ĸ���㣬������������
    while (searchDistrict->parent != NULL)
    {
        //����������Ŀ�����������
        double districtDistance = abs(goal[(d+1)%k] - searchDistrict->parent->root[(d+1)%k]);

        //���������������Ŀ����������롱�ȡ���ǰ�������Ŀ���ľ��롱�̣���������
        //�����ڿ��ܴ��ھ���Ŀ�������ĵ�
        if (districtDistance < currentDistance )//&& !searchDistrict->isEmpty()
        {

            double parentDistance = measureDistance(goal, searchDistrict->parent->root, 0);

            if (parentDistance < currentDistance)
            {
                currentDistance = parentDistance;
                currentTree = searchDistrict->parent;
                currentNearest = currentTree->root;
            }
            if (!searchDistrict->isEmpty())
            {
                double rootDistance = measureDistance(goal, searchDistrict->root, 0);
                if (rootDistance < currentDistance)
                {
                    currentDistance = rootDistance;
                    currentTree = searchDistrict;
                    currentNearest = currentTree->root;
                }
            }
            if (searchDistrict->leftChild != NULL)
            {
                double leftDistance = measureDistance(goal, searchDistrict->leftChild->root, 0);
                if (leftDistance < currentDistance)
                {
                    currentDistance = leftDistance;
                    currentTree = searchDistrict;
                    currentNearest = currentTree->root;
                }
            }
            if (searchDistrict->rightChild != NULL)
            {
                double rightDistance = measureDistance(goal, searchDistrict->rightChild->root, 0);
                if (rightDistance < currentDistance)
                {
                    currentDistance = rightDistance;
                    currentTree = searchDistrict;
                    currentNearest = currentTree->root;
                }
            }
        }//end if

        if (searchDistrict->parent->parent != NULL)
        {
            searchDistrict = searchDistrict->parent->isLeft()? 
                            searchDistrict->parent->parent->rightChild:
                            searchDistrict->parent->parent->leftChild;
        }
        else
        {
            searchDistrict = searchDistrict->parent;
        }
        ++d;
    }//end while
    return currentNearest;
}

int main()
{
    vector<vector<double> > train(6, vector<double>(2, 0));
    for (unsigned i = 0; i < 6; ++i)
        for (unsigned j = 0; j < 2; ++j)
            train[i][j] = data[i][j];

    KdTree* kdTree = new KdTree;
    buildKdTree(kdTree, train, 0);

    printKdTree(kdTree, 0);

    vector<double> goal;
    goal.push_back(3);
    goal.push_back(4.5);
    vector<double> nearestNeighbor = searchNearestNeighbor(goal, kdTree);
    vector<double>::iterator beg = nearestNeighbor.begin();
    cout << "The nearest neighbor is: ";
    while(beg != nearestNeighbor.end()) cout << *beg++ << ",";
    cout << endl;
    return 0;
}