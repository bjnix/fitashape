xof 0303txt 0032

Frame Root {
  FrameTransformMatrix {
     1.000000, 0.000000, 0.000000, 0.000000,
     0.000000,-0.000000, 1.000000, 0.000000,
     0.000000, 1.000000, 0.000000, 0.000000,
     0.000000, 0.000000, 0.000000, 1.000000;;
  }
  Frame Plane {
    FrameTransformMatrix {
       1.000000, 0.000000, 0.000000, 0.000000,
       0.000000,-0.000001, 1.000000, 0.000000,
       0.000000,-1.000000,-0.000001, 0.000000,
       0.000000, 0.000000, 0.000000, 1.000000;;
    }
    Mesh { // Plane mesh
      4;
      -0.924528;-0.500000; 0.000000;,
       0.924528;-0.500000; 0.000000;,
       0.924528; 0.500000; 0.000000;,
      -0.924528; 0.500000; 0.000000;;
      1;
      4;3,2,1,0;;
      MeshNormals { // Plane normals
        1;
         0.000000; 0.000000; 1.000000;;
        1;
        4;0,0,0,0;;
      } // End of Plane normals
      MeshTextureCoords { // Plane UV coordinates
        4;
         0.000000; 1.000000;,
         1.000000; 1.000000;,
         1.000000; 0.000000;,
         0.000000; 0.000000;;
      } // End of Plane UV coordinates
      MeshMaterialList { // Plane material list
        1;
        1;
        0;;
        Material Material {
           0.640000; 0.640000; 0.640000; 1.000000;;
           96.078431;
           0.500000; 0.500000; 0.500000;;
           0.000000; 0.000000; 0.000000;;
          TextureFilename {"New-Button.png";}
        }
      } // End of Plane material list
    } // End of Plane mesh
  } // End of Plane
} // End of Root
