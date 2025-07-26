for img in *.png; do
   if [ -f "$img" ]; then
      png2tim "$img";
      echo "Converted $img to TIM format.";
   fi;
done
