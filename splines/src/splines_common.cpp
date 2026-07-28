#include "splines_common.h"

namespace baltam::splines {

    // spapi 和 fn2fm 所需函数 aptknt 的实现
    std::vector<double> aptknt(std::vector<double> knots, int k) {
	if (k > static_cast<int>(knots.size())){
	    k = static_cast<int>(knots.size());
	}
        std::vector<double> res;
        res.push_back(knots[0]);
        std::vector<double> tmp = aveknt(knots, k);
        for (size_t i = 0; i < tmp.size(); i++)
            res.push_back(tmp[i]);
        res.push_back(knots[knots.size()-1]);
        return augknt(res, k);
    }
    
    // aptknt 所需的 aveknt 函数的实现
    std::vector<double> aveknt(std::vector<double> knots, int k) {
        std::vector<double> res;
        for (int i = 1; i <= static_cast<int>(knots.size())-k; i++) {
            double sum = 0;
            for (int j = i; j <= i+k-2; j++)
                sum += knots[j];
            sum /= k-1;
            res.push_back(sum);
        }
        return res;
    }

    // augknt 所需的 augknt 函数的实现
    std::vector<double> augknt(std::vector<double> knots, int k) {
        std::vector<double> res; 
	double a = knots[0];
	double b = knots[knots.size()-1];
        for (int i = 0; i < k; i++)
            res.push_back(a);
        for (size_t i = 1; i < knots.size()-1; i++){
	    if((knots[i] != a)&&(knots[i] != b)){
		res.push_back(knots[i]);
	    }
	}
        for (int i = 0; i < k; i++)
            res.push_back(b);
        return res;
    }

    // 判断一个结构体是否为一个合法的样条结构体，pp or B
    bool is_legal_spline_structure(const baltam::structure &s) {
        bool A = s.isfield("form");
        if(!A)
            return false; // 两种样条结构体都得有 form 属性
        // pp 格式的判断
        if(s.get_field("form")->as_string() == "pp")
        {
            bool right_fields = s.isfield("form") &&  // 这些必须是输入结构体的元素，否则不是样条结构体
                                s.isfield("breaks") &&
                                s.isfield("coefs") &&
                                s.isfield("order") &&
                                s.isfield("pieces") &&
                                s.isfield("dim");
            if(!right_fields || s.data().size()!= 6)
                return false; // 如果以上属性不是全部拥有，一定不合法 // 必须有且只有以上 6 个属性，否则不合法
            auto Breaks = s.get_field("breaks")->get<baltam::matrix<double>>();
            auto Coefs = s.get_field("coefs")->get<baltam::matrix<double>>();
            auto Order = s.get_field("order")->as_int();
            auto Dim = s.get_field("dim")->as_int();
            auto Pieces = s.get_field("pieces")->as_int();

            bool right_size = (Breaks->cols() == Coefs->rows() + 1) &&                // 节点的个数等于多项式个数加一
                              (Order == static_cast<int>(Coefs->cols())) &&           // 样条阶数等于分片多项式的次数
                              (Pieces == static_cast<int>( Breaks->cols() - 1)) &&    // 分片多项式的个数等于结点个数-1
                              (Dim > 0);                                              // 维数大于0
            return right_size;
        }
        // B 格式的判断
        else if(s.get_field("form")->as_string() == "B-")
        {
            bool right_fields = s.isfield("form") &&  // 这些必须是输入结构体的元素，否则不是样条结构体
                                s.isfield("knots") && // knots 是结点序列，可能有重复结点，与 pp 中 breaks 对应
                                s.isfield("coefs") && // coefs 是每个 B 样条基函数前的系数
                                s.isfield("number") &&// B 样条基函数的个数
                                s.isfield("order") && // 阶数
                                s.isfield("dim");     // 维数
            if(!right_fields || s.data().size()!= 6)
                return false; // 如果以上属性不是全部拥有，一定不合法 // 必须有且只有以上 6 个属性，否则不合法
            auto Knots = s.get_field("knots")->get<matrix<double>>();
            auto Coefs = s.get_field("coefs")->get<baltam::matrix<double>>();
            auto Order = s.get_field("order")->as_int();
            auto Dim = s.get_field("dim")->as_int();
            auto Number = s.get_field("number")->as_int();

            bool right_size = (Number == Coefs->size()) &&                 // B 样条基函数的个数必须等于其系数的个数
                              (Order == Knots->size() - Coefs->size()) &&  // 阶数为结点个数（算重复结点）减去 B 样条基函数的个数
                              (Dim > 0);                                   // 维数大于0
            return right_size;
        }
        else
            return false; // 只有以上两种合法的样条结构体
    } // end of is_legal_spline_structure

} // end of namespace baltam::splines
