#include <bits/stdc++.h>
using namespace std;
#define x first
#define y second
#define OUTSIDE 0
#define INSIDE 1
#define	INTERSECT 2
#define CONTAIN 3

struct QuadTreeNode {
	double xMin, xMax, yMin, yMax;
	bool isLeaf = false;
	vector<pair<double, double> > pointSet;
	QuadTreeNode* nxt[4];
	//2 3
	//0 1
	QuadTreeNode* pre;
	QuadTreeNode() {}
	QuadTreeNode(double _x1, double _x2, double _y1, double _y2) {
		xMin = _x1, xMax = _x2, yMin = _y1, yMax = _y2;
	}

	void split() {
		double xMid = (xMin + xMax) / 2, yMid = (yMin + yMax) / 2;
		nxt[0] = new QuadTreeNode(xMin, xMid, yMin, yMid);
		nxt[1] = new QuadTreeNode(xMid, xMax, yMin, yMid);
		nxt[2] = new QuadTreeNode(xMid, xMax, yMid, yMax);
		nxt[3] = new QuadTreeNode(xMin, xMid, yMid, yMax);
		nxt[0]->pre = this, nxt[1]->pre = this, nxt[2]->pre = this, nxt[3]->pre = this;
		nxt[0]->isLeaf = nxt[1]->isLeaf = nxt[2]->isLeaf = nxt[3]->isLeaf = true;
		for (auto p: pointSet) {
			int ind;
			if (p.y <= yMid) {
				if (p.x <= xMid) 
					ind = 0;
				else
					ind = 1;
			} else {
				if (p.x <= xMid)
					ind = 3;
				else
					ind = 2;
			}
			nxt[ind]->pointSet.push_back(p);
		}
		isLeaf = false;
	}

};

template<typename T, typename Y>
struct comparePairSecondFunc {
	bool operator() (std::pair<T, Y> p, std::pair<T, Y> q)  {
		return p.second > q.second;
	}
};

double calcDistanceSquare(double x1, double y1, double x2, double y2) {

	return (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
}

double calcDistanceSquare(double xLow, double xHigh, double yLow, double yHigh, double xQ, double yQ) {
	if (yQ < yLow) {
		if (xQ < xLow) {
			return calcDistanceSquare(xLow, yLow, xQ, yQ);
		} else if (xQ < xHigh) {
			return yLow - yQ;
		} else {
			return calcDistanceSquare(xHigh, yLow, xQ, yQ);
		}
	} else if (yQ < yHigh) {
		if (xQ < xLow) {
			return xLow - xQ;
		} else if (xQ < xHigh) {
			return 0;
		} else {
			return xQ - xHigh;
		}
	} else {
		if (xQ < xLow) {
			return calcDistanceSquare(xLow, yHigh, xQ, yQ);
		} else if (xQ < xHigh) {
			return yQ - yHigh;
		} else {
			return calcDistanceSquare(xHigh, yHigh, xQ, yQ);
		}
	}
}

struct QuadTree {
	QuadTreeNode* root;
	int threshold;
	double xMin, xMax, yMin, yMax;
	void build(double _x1, double _x2, double _y1, double _y2, int _t) {
		root = new QuadTreeNode();
		root->isLeaf = true;
		threshold = _t;
		xMin = _x1, xMax = _x2, yMin = _y1, yMax = _y2;
		root->xMin = _x1, root->xMax = _x2, root->yMin = _y1, root->yMax = _y2;
	}
	void insert(double x, double y) {
		QuadTreeNode* cur = root;
		while (1) {
			if (cur->isLeaf) {
				cur->pointSet.push_back(make_pair(x, y));
				if (cur->pointSet.size() > threshold) {
					cur->split();
				}
				break;
			}
			int ind;
			if (y <= (cur->yMin + cur->yMax) / 2) {
				if (x <= (cur->xMin + cur->xMax) / 2) 
					ind = 0;
				else
					ind = 1;
			} else {
				if (x <= (cur->xMin + cur->xMax) / 2)
					ind = 3;
				else
					ind = 2;
			}
			cur = cur->nxt[ind];
		}
	}

	vector<pair<double, double> >getPointsInRect(vector<pair<double, double> > pointSet, double xMin, double xMax, double yMin, double yMax) {
		vector<pair<double, double> > res;
		for (auto p: pointSet) {
			if (p.x >= xMin && p.x <= xMax && p.y >= yMin && p.y <= yMax)
				res.push_back(p);
		}
		return res;
	}
	//rect1: x1,x2,y1,y2  rect2: x3,x4,y3,y4
	//rect1 OUTSIDE/INTERSECT/INSIDE/CONTAIN rect2
	int rectRelationship(double x1, double x2, double y1, double y2, double x3, double x4, double y3, double y4) {
		if (max(x1, x3) <= min(x2, x4) && max(y1, y3) <= min(y2, y4)) {
			if (x1 <= x3 && x2 >= x4 && y1 <= y3 && y2 >= y4) 
				return CONTAIN;
			else if (x1 >= x3 && x2 <= x4 && y1 >= y3 && y2 <= y4) 
				return INSIDE;
			else
				return INTERSECT;
		} else {
			return OUTSIDE;
		}
	}

	vector<pair<double, double> > rangeQuery(QuadTreeNode* cur, double qxMin, double qxMax, double qyMin, double qyMax) {
		vector<pair<double, double> > ans;
		int rela = rectRelationship(cur->xMin, cur->xMax, cur->yMin, cur->yMax, qxMin, qxMax, qyMin, qyMax);
		if (cur->isLeaf) {
			if (rela == INSIDE) {
				return cur->pointSet;
			} else if (rela == OUTSIDE) {
				return ans;
			} else {
				return getPointsInRect(cur->pointSet, qxMin, qxMax, qyMin, qyMax);
			}
		} else {
			if (rela == OUTSIDE) 
				return ans;
			else {
				for (int i = 0; i < 4; ++i) {
					vector<pair<double, double> > temp = rangeQuery(cur->nxt[i], qxMin, qxMax, qyMin, qyMax);
					ans.insert(ans.end(), temp.begin(), temp.end());
				}
				return ans;
			}
		}
	}

	vector<pair<double, double> > querykNearestNeighbour(double xQ, double yQ, int K) {
		vector<pair<double, double> > ret;
		priority_queue<pair<QuadTreeNode*, double>,
					   vector<pair<QuadTreeNode*, double> >, 
					   comparePairSecondFunc<QuadTreeNode*, double> > queNode;
		priority_queue<pair<pair<double, double>, double>, 
					   vector<pair<pair<double, double>, double> >, 
					   comparePairSecondFunc<pair<double, double>, double> > queElement;
		queNode.push(make_pair(root, calcDistanceSquare(root->xMin,root->xMax, root->yMin, root->yMax, xQ, yQ)));
		while (!queNode.empty() || !queElement.empty()) {
			if (queElement.empty() || (!queNode.empty() && queNode.top().second < queElement.top().second)) {
				QuadTreeNode* curNode = queNode.top().first;
				queNode.pop();
				if (curNode->isLeaf) {
					for (auto p: curNode->pointSet) {
						queElement.push(make_pair(p, calcDistanceSquare(p.x, p.y, xQ, yQ)));
					}
				} else {
					for (int i = 0; i < 4; ++i) {
						QuadTreeNode* temp = curNode->nxt[i];
						queNode.push(make_pair(temp, calcDistanceSquare(temp->xMin, temp->xMax, temp->yMin, temp->yMax, xQ, yQ)));
					}
				}
				
			} else {
				ret.push_back(queElement.top().first);
				queElement.pop();
				if (ret.size() == K)
					return ret;
			}
		}
	}

	~QuadTree() {
		function<void(QuadTreeNode*)> func;
		func = [&](QuadTreeNode* currentRoot) {
			if (!(currentRoot->isLeaf)) {
				for (int i = 0; i < 4; ++i) {
					func(currentRoot->nxt[i]);
				}
			}
			delete currentRoot;
		};
		func(root);
	}
	
};


int main() {
	QuadTree G = QuadTree();
	G.build(0, 16, 0, 16, 1);
	G.insert(3, 7);
	G.insert(4, 2);
	G.insert(13, 9);
	G.insert(7, 6);
	G.insert(6, 15);
	G.insert(3, 2);
	auto ans = G.rangeQuery(G.root, 2, 15, 1, 11);
	for (auto p: ans) {
		cout << p.x << "  " << p.y << endl;
	}
	cout << "-------------------" << endl;
	ans = G.querykNearestNeighbour(3, 6, 4);
	for (auto p: ans) {
		cout << p.x << "  " << p.y << endl;
	}
}