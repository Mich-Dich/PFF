

namespace PFF {

    namespace composite_mesh {

        // @brief Represents all possible positions of a 3x3x3 cube's elements.
        // @brief This enumeration defines the positions of individual elements (cubies) within a 3x3x3 cube structure.
        // @brief Each enumerator name follows the format `<Category>_<XYZ>`, where:
        // @param `<Category>` is a single character indicating the type of position:
        // @param   `C`: Corner
        // @param   `E`: Edge
        // @param   `F`: Face (center of a face)
        // @param   `M`: Middle (center of the cube)
        // @param `<XYZ>` are digits representing the coordinates along the X, Y, and Z axes respectively.
        // 
        // @note For example:
        // @note - C_000: denotes the corner at position (0,0,0).
        // @note - F_011: denotes the center of the face at position (0,1,1).
        // @note - M_111: denotes the center of the cube at position (1,1,1).
        enum class position {

            C_000 = 0,
            E_001,
            C_002,
            E_010,
            F_011,
            E_012,
            C_020,
            E_021,
            C_022,

            E_100,
            F_101,
            E_102,
            F_110,
            M_111,
            F_112,
            E_120,
            F_121,
            E_122,

            C_200,
            E_201,
            C_202,
            E_210,
            F_211,
            E_212,
            C_220,
            E_221,
            C_222,
        };

        

    }




    
}