xof 0303txt 0032
template FVFData {
 <b6e70a0e-8ef9-4e83-94ad-ecc8b0c04897>
 DWORD dwFVF;
 DWORD nDWords;
 array DWORD data[nDWords];
}

template EffectInstance {
 <e331f7e4-0559-4cc2-8e99-1cec1657928f>
 STRING EffectFilename;
 [...]
}

template EffectParamFloats {
 <3014b9a0-62f5-478c-9b86-e4ac9f4e418b>
 STRING ParamName;
 DWORD nFloats;
 array FLOAT Floats[nFloats];
}

template EffectParamString {
 <1dbc4c88-94c1-46ee-9076-2c28818c9481>
 STRING ParamName;
 STRING Value;
}

template EffectParamDWord {
 <e13963bc-ae51-4c5d-b00f-cfa3a9d97ce5>
 STRING ParamName;
 DWORD Value;
}


Mesh Box03 {
 6;
 -18.315456;6.531045;0.000034;,
 -14.275673;6.531045;0.000034;,
 -18.315456;0.998654;0.000035;,
 -14.275673;0.998654;0.000035;,
 -18.315456;0.998654;0.000035;,
 -14.275673;6.531045;0.000034;;
 2;
 3;0,1,2;,
 3;3,4,5;;

 MeshNormals  {
  6;
  0.000000;0.000000;1.000000;,
  0.000000;0.000000;1.000000;,
  0.000000;0.000000;1.000000;,
  0.000000;0.000000;1.000000;,
  0.000000;0.000000;1.000000;,
  0.000000;0.000000;1.000000;;
  2;
  3;0,1,2;,
  3;3,4,5;;
 }

 MeshMaterialList  {
  1;
  2;
  0,
  0;

  Material Standard_3 {
   1.000000;1.000000;1.000000;1.000000;;
   0.000000;
   1.000000;1.000000;1.000000;;
   0.000000;0.000000;0.000000;;

   EffectInstance {
    "C:\\Work\\AdvancedDecals\\data\\scene.fx";

    EffectParamString {
     "g_tNormal";
     "decal_set_01_brik_03_tgspace.png";
    }

    EffectParamString {
     "g_tDiffuse";
     "decal_set_01_brik_03_diffuse.png";
    }

    EffectParamString {
     "g_tSpecular";
     "None";
    }

    EffectParamString {
     "g_tOpacity";
     "decal_set_01_brik_03_opacity.png";
    }

    EffectParamDWord {
     "technique";
     2;
    }
   }
  }
 }

 MeshTextureCoords  {
  6;
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  0.000000;0.000000;;
 }
}

Mesh Box04 {
 6;
 -12.116186;7.653997;0.000034;,
 -8.076403;7.653997;0.000034;,
 -12.116186;2.121606;0.000035;,
 -8.076403;2.121606;0.000035;,
 -12.116186;2.121606;0.000035;,
 -8.076403;7.653997;0.000034;;
 2;
 3;0,1,2;,
 3;3,4,5;;

 MeshNormals  {
  6;
  0.000000;0.000000;1.000000;,
  0.000000;0.000000;1.000000;,
  0.000000;0.000000;1.000000;,
  0.000000;0.000000;1.000000;,
  0.000000;0.000000;1.000000;,
  0.000000;0.000000;1.000000;;
  2;
  3;0,1,2;,
  3;3,4,5;;
 }

 MeshMaterialList  {
  1;
  2;
  0,
  0;

  Material Standard_3 {
   1.000000;1.000000;1.000000;1.000000;;
   0.000000;
   1.000000;1.000000;1.000000;;
   0.000000;0.000000;0.000000;;

   EffectInstance {
    "C:\\Work\\AdvancedDecals\\data\\scene.fx";

    EffectParamString {
     "g_tNormal";
     "decal_set_01_brik_03_tgspace.png";
    }

    EffectParamString {
     "g_tDiffuse";
     "decal_set_01_brik_03_diffuse.png";
    }

    EffectParamString {
     "g_tSpecular";
     "None";
    }

    EffectParamString {
     "g_tOpacity";
     "decal_set_01_brik_03_opacity.png";
    }

    EffectParamDWord {
     "technique";
     2;
    }
   }
  }
 }

 MeshTextureCoords  {
  6;
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  0.000000;0.000000;;
 }
}

Mesh Box05 {
 6;
 -20.272425;6.001094;4.228408;,
 -20.272425;6.001094;0.188625;,
 -20.272425;0.468703;4.228408;,
 -20.272425;0.468703;0.188625;,
 -20.272425;0.468703;4.228408;,
 -20.272425;6.001094;0.188625;;
 2;
 3;0,1,2;,
 3;3,4,5;;

 MeshNormals  {
  6;
  1.000000;0.000000;-0.000000;,
  1.000000;0.000000;-0.000000;,
  1.000000;0.000000;-0.000000;,
  1.000000;0.000000;-0.000000;,
  1.000000;0.000000;-0.000000;,
  1.000000;0.000000;-0.000000;;
  2;
  3;0,1,2;,
  3;3,4,5;;
 }

 MeshMaterialList  {
  1;
  2;
  0,
  0;

  Material Standard_3 {
   1.000000;1.000000;1.000000;1.000000;;
   0.000000;
   1.000000;1.000000;1.000000;;
   0.000000;0.000000;0.000000;;

   EffectInstance {
    "C:\\Work\\AdvancedDecals\\data\\scene.fx";

    EffectParamString {
     "g_tNormal";
     "decal_set_01_brik_03_tgspace.png";
    }

    EffectParamString {
     "g_tDiffuse";
     "decal_set_01_brik_03_diffuse.png";
    }

    EffectParamString {
     "g_tSpecular";
     "None";
    }

    EffectParamString {
     "g_tOpacity";
     "decal_set_01_brik_03_opacity.png";
    }

    EffectParamDWord {
     "technique";
     2;
    }
   }
  }
 }

 MeshTextureCoords  {
  6;
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  0.000000;0.000000;;
 }
}

Mesh Box06 {
 6;
 -14.275673;5.965151;4.246479;,
 -18.315456;5.965151;4.246480;,
 -14.275673;0.432760;4.246479;,
 -18.315456;0.432760;4.246479;,
 -14.275673;0.432760;4.246479;,
 -18.315456;5.965151;4.246480;;
 2;
 3;0,1,2;,
 3;3,4,5;;

 MeshNormals  {
  6;
  -0.000000;0.000000;-1.000000;,
  -0.000000;0.000000;-1.000000;,
  -0.000000;0.000000;-1.000000;,
  -0.000000;0.000000;-1.000000;,
  -0.000000;0.000000;-1.000000;,
  -0.000000;0.000000;-1.000000;;
  2;
  3;0,1,2;,
  3;3,4,5;;
 }

 MeshMaterialList  {
  1;
  2;
  0,
  0;

  Material Standard_3 {
   1.000000;1.000000;1.000000;1.000000;;
   0.000000;
   1.000000;1.000000;1.000000;;
   0.000000;0.000000;0.000000;;

   EffectInstance {
    "C:\\Work\\AdvancedDecals\\data\\scene.fx";

    EffectParamString {
     "g_tNormal";
     "decal_set_01_brik_03_tgspace.png";
    }

    EffectParamString {
     "g_tDiffuse";
     "decal_set_01_brik_03_diffuse.png";
    }

    EffectParamString {
     "g_tSpecular";
     "None";
    }

    EffectParamString {
     "g_tOpacity";
     "decal_set_01_brik_03_opacity.png";
    }

    EffectParamDWord {
     "technique";
     2;
    }
   }
  }
 }

 MeshTextureCoords  {
  6;
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  0.000000;0.000000;;
 }
}

Mesh Box07 {
 6;
 -6.561340;8.084445;4.246479;,
 -10.601124;8.084445;4.246480;,
 -6.561340;2.552054;4.246479;,
 -10.601124;2.552054;4.246479;,
 -6.561340;2.552054;4.246479;,
 -10.601124;8.084445;4.246480;;
 2;
 3;0,1,2;,
 3;3,4,5;;

 MeshNormals  {
  6;
  -0.000000;0.000000;-1.000000;,
  -0.000000;0.000000;-1.000000;,
  -0.000000;0.000000;-1.000000;,
  -0.000000;0.000000;-1.000000;,
  -0.000000;0.000000;-1.000000;,
  -0.000000;0.000000;-1.000000;;
  2;
  3;0,1,2;,
  3;3,4,5;;
 }

 MeshMaterialList  {
  1;
  2;
  0,
  0;

  Material Standard_3 {
   1.000000;1.000000;1.000000;1.000000;;
   0.000000;
   1.000000;1.000000;1.000000;;
   0.000000;0.000000;0.000000;;

   EffectInstance {
    "C:\\Work\\AdvancedDecals\\data\\scene.fx";

    EffectParamString {
     "g_tNormal";
     "decal_set_01_brik_03_tgspace.png";
    }

    EffectParamString {
     "g_tDiffuse";
     "decal_set_01_brik_03_diffuse.png";
    }

    EffectParamString {
     "g_tSpecular";
     "None";
    }

    EffectParamString {
     "g_tOpacity";
     "decal_set_01_brik_03_opacity.png";
    }

    EffectParamDWord {
     "technique";
     2;
    }
   }
  }
 }

 MeshTextureCoords  {
  6;
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  0.000000;0.000000;;
 }
}

Mesh Box08 {
 6;
 -20.129942;8.093565;0.000034;,
 -14.415167;8.093565;0.000034;,
 -20.129942;5.120403;0.000035;,
 -14.415167;5.120403;0.000035;,
 -20.129942;5.120403;0.000035;,
 -14.415167;8.093565;0.000034;;
 2;
 3;0,1,2;,
 3;3,4,5;;

 MeshNormals  {
  6;
  0.000000;0.000000;1.000000;,
  0.000000;0.000000;1.000000;,
  0.000000;0.000000;1.000000;,
  0.000000;0.000000;1.000000;,
  0.000000;0.000000;1.000000;,
  0.000000;0.000000;1.000000;;
  2;
  3;0,1,2;,
  3;3,4,5;;
 }

 MeshMaterialList  {
  1;
  2;
  0,
  0;

  Material Standard_2 {
   1.000000;1.000000;1.000000;1.000000;;
   0.000000;
   1.000000;1.000000;1.000000;;
   0.000000;0.000000;0.000000;;

   EffectInstance {
    "C:\\Work\\AdvancedDecals\\data\\scene.fx";

    EffectParamString {
     "g_tNormal";
     "decal_set_01_run_02_tgspace.png";
    }

    EffectParamString {
     "g_tDiffuse";
     "decal_set_01_run_02_diffuse.png";
    }

    EffectParamString {
     "g_tSpecular";
     "None";
    }

    EffectParamString {
     "g_tOpacity";
     "decal_set_01_run_02_opacity.png";
    }

    EffectParamDWord {
     "technique";
     2;
    }
   }
  }
 }

 MeshTextureCoords  {
  6;
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  0.000000;0.000000;;
 }
}

Mesh Box09 {
 6;
 -10.150467;8.093565;0.000034;,
 -4.435692;8.093565;0.000034;,
 -10.150467;5.120403;0.000035;,
 -4.435692;5.120403;0.000035;,
 -10.150467;5.120403;0.000035;,
 -4.435692;8.093565;0.000034;;
 2;
 3;0,1,2;,
 3;3,4,5;;

 MeshNormals  {
  6;
  0.000000;0.000000;1.000000;,
  0.000000;0.000000;1.000000;,
  0.000000;0.000000;1.000000;,
  0.000000;0.000000;1.000000;,
  0.000000;0.000000;1.000000;,
  0.000000;0.000000;1.000000;;
  2;
  3;0,1,2;,
  3;3,4,5;;
 }

 MeshMaterialList  {
  1;
  2;
  0,
  0;

  Material Standard_2 {
   1.000000;1.000000;1.000000;1.000000;;
   0.000000;
   1.000000;1.000000;1.000000;;
   0.000000;0.000000;0.000000;;

   EffectInstance {
    "C:\\Work\\AdvancedDecals\\data\\scene.fx";

    EffectParamString {
     "g_tNormal";
     "decal_set_01_run_02_tgspace.png";
    }

    EffectParamString {
     "g_tDiffuse";
     "decal_set_01_run_02_diffuse.png";
    }

    EffectParamString {
     "g_tSpecular";
     "None";
    }

    EffectParamString {
     "g_tOpacity";
     "decal_set_01_run_02_opacity.png";
    }

    EffectParamDWord {
     "technique";
     2;
    }
   }
  }
 }

 MeshTextureCoords  {
  6;
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  0.000000;0.000000;;
 }
}

Mesh Box10 {
 6;
 -13.749817;0.023134;0.027459;,
 -8.035042;0.023134;0.027459;,
 -13.749817;0.023134;-2.945703;,
 -8.035042;0.023134;-2.945703;,
 -13.749817;0.023134;-2.945703;,
 -8.035042;0.023134;0.027459;;
 2;
 3;0,1,2;,
 3;3,4,5;;

 MeshNormals  {
  6;
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;;
  2;
  3;0,1,2;,
  3;3,4,5;;
 }

 MeshMaterialList  {
  1;
  2;
  0,
  0;

  Material Standard_2 {
   1.000000;1.000000;1.000000;1.000000;;
   0.000000;
   1.000000;1.000000;1.000000;;
   0.000000;0.000000;0.000000;;

   EffectInstance {
    "C:\\Work\\AdvancedDecals\\data\\scene.fx";

    EffectParamString {
     "g_tNormal";
     "decal_set_01_run_02_tgspace.png";
    }

    EffectParamString {
     "g_tDiffuse";
     "decal_set_01_run_02_diffuse.png";
    }

    EffectParamString {
     "g_tSpecular";
     "None";
    }

    EffectParamString {
     "g_tOpacity";
     "decal_set_01_run_02_opacity.png";
    }

    EffectParamDWord {
     "technique";
     2;
    }
   }
  }
 }

 MeshTextureCoords  {
  6;
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  0.000000;0.000000;;
 }
}

Mesh Box11 {
 6;
 -23.756962;0.014433;-2.129744;,
 -19.717178;0.014433;-2.129744;,
 -23.756962;0.014433;-7.662135;,
 -19.717178;0.014433;-7.662135;,
 -23.756962;0.014433;-7.662135;,
 -19.717178;0.014433;-2.129744;;
 2;
 3;0,1,2;,
 3;3,4,5;;

 MeshNormals  {
  6;
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;;
  2;
  3;0,1,2;,
  3;3,4,5;;
 }

 MeshMaterialList  {
  1;
  2;
  0,
  0;

  Material Standard_3 {
   1.000000;1.000000;1.000000;1.000000;;
   0.000000;
   1.000000;1.000000;1.000000;;
   0.000000;0.000000;0.000000;;

   EffectInstance {
    "C:\\Work\\AdvancedDecals\\data\\scene.fx";

    EffectParamString {
     "g_tNormal";
     "decal_set_01_brik_03_tgspace.png";
    }

    EffectParamString {
     "g_tDiffuse";
     "decal_set_01_brik_03_diffuse.png";
    }

    EffectParamString {
     "g_tSpecular";
     "None";
    }

    EffectParamString {
     "g_tOpacity";
     "decal_set_01_brik_03_opacity.png";
    }

    EffectParamDWord {
     "technique";
     2;
    }
   }
  }
 }

 MeshTextureCoords  {
  6;
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  0.000000;0.000000;;
 }
}

Mesh Box12 {
 6;
 -18.813995;0.014433;-1.150267;,
 -16.119871;0.014433;-4.160507;,
 -22.936449;0.014433;-4.839810;,
 -20.242325;0.014433;-7.850049;,
 -22.936449;0.014433;-4.839810;,
 -16.119871;0.014433;-4.160507;;
 2;
 3;0,1,2;,
 3;3,4,5;;

 MeshNormals  {
  6;
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;;
  2;
  3;0,1,2;,
  3;3,4,5;;
 }

 MeshMaterialList  {
  1;
  2;
  0,
  0;

  Material Standard_3 {
   1.000000;1.000000;1.000000;1.000000;;
   0.000000;
   1.000000;1.000000;1.000000;;
   0.000000;0.000000;0.000000;;

   EffectInstance {
    "C:\\Work\\AdvancedDecals\\data\\scene.fx";

    EffectParamString {
     "g_tNormal";
     "decal_set_01_brik_03_tgspace.png";
    }

    EffectParamString {
     "g_tDiffuse";
     "decal_set_01_brik_03_diffuse.png";
    }

    EffectParamString {
     "g_tSpecular";
     "None";
    }

    EffectParamString {
     "g_tOpacity";
     "decal_set_01_brik_03_opacity.png";
    }

    EffectParamDWord {
     "technique";
     2;
    }
   }
  }
 }

 MeshTextureCoords  {
  6;
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  0.000000;0.000000;;
 }
}

Mesh Box13 {
 6;
 -13.821383;0.014433;-2.466069;,
 -13.800611;0.014433;-6.505798;,
 -19.353703;0.014433;-2.494518;,
 -19.332928;0.014433;-6.534247;,
 -19.353703;0.014433;-2.494518;,
 -13.800611;0.014433;-6.505798;;
 2;
 3;0,1,2;,
 3;3,4,5;;

 MeshNormals  {
  6;
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;;
  2;
  3;0,1,2;,
  3;3,4,5;;
 }

 MeshMaterialList  {
  1;
  2;
  0,
  0;

  Material Standard_3 {
   1.000000;1.000000;1.000000;1.000000;;
   0.000000;
   1.000000;1.000000;1.000000;;
   0.000000;0.000000;0.000000;;

   EffectInstance {
    "C:\\Work\\AdvancedDecals\\data\\scene.fx";

    EffectParamString {
     "g_tNormal";
     "decal_set_01_brik_03_tgspace.png";
    }

    EffectParamString {
     "g_tDiffuse";
     "decal_set_01_brik_03_diffuse.png";
    }

    EffectParamString {
     "g_tSpecular";
     "None";
    }

    EffectParamString {
     "g_tOpacity";
     "decal_set_01_brik_03_opacity.png";
    }

    EffectParamDWord {
     "technique";
     2;
    }
   }
  }
 }

 MeshTextureCoords  {
  6;
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  0.000000;0.000000;;
 }
}

Mesh Box14 {
 6;
 -10.116917;0.014433;-5.235479;,
 -13.144121;0.014433;-7.910526;,
 -13.780333;0.014433;-1.089790;,
 -16.807537;0.014433;-3.764838;,
 -13.780333;0.014433;-1.089790;,
 -13.144121;0.014433;-7.910526;;
 2;
 3;0,1,2;,
 3;3,4,5;;

 MeshNormals  {
  6;
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;;
  2;
  3;0,1,2;,
  3;3,4,5;;
 }

 MeshMaterialList  {
  1;
  2;
  0,
  0;

  Material Standard_3 {
   1.000000;1.000000;1.000000;1.000000;;
   0.000000;
   1.000000;1.000000;1.000000;;
   0.000000;0.000000;0.000000;;

   EffectInstance {
    "C:\\Work\\AdvancedDecals\\data\\scene.fx";

    EffectParamString {
     "g_tNormal";
     "decal_set_01_brik_03_tgspace.png";
    }

    EffectParamString {
     "g_tDiffuse";
     "decal_set_01_brik_03_diffuse.png";
    }

    EffectParamString {
     "g_tSpecular";
     "None";
    }

    EffectParamString {
     "g_tOpacity";
     "decal_set_01_brik_03_opacity.png";
    }

    EffectParamDWord {
     "technique";
     2;
    }
   }
  }
 }

 MeshTextureCoords  {
  6;
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  0.000000;0.000000;;
 }
}