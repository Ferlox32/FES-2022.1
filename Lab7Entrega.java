import java.util.Random;

class AchaPar extends Thread{

    private int id;
    private int[] vetor;
    public static int qtd = 0;

    public AchaPar(int id, int[] vetor){
        this.id = id;
        this.vetor = vetor;
    }

    public synchronized void run(){
        for(int i = id; i < Pares.tam; i += Pares.N){
            if(vetor[i] % 2 == 0){
                qtd++;
            }
        }
    }
}

class Pares{

    public static final int N = 4;
    static final int tam = 1000;

    public static void main (String[] args){

        Thread[] threads = new Thread[N];
        int[] vetor = new int[tam];
        int i, qtd = 0;

        Random gerador = new Random();
        for(i = 0; i < tam; i++){
            vetor[i] = gerador.nextInt(tam);
        }

        for(i = 0; i < tam; i++){
            if(vetor[i] % 2 == 0){
                qtd++;
            }
        }

        for(i = 0; i < threads.length; i++){
            threads[i] = new Thread(new AchaPar(i, vetor));
        }

        for(i = 0; i < threads.length; i++){
            threads[i].start();
        }

        for(i = 0; i < threads.length; i++){
            try{
                threads[i].join();
            }
            catch(InterruptedException e)
            {
                return;}
        }

        System.out.printf("[%d,", vetor[0]);
        for(i = 1; i < tam - 1; i++){
            System.out.printf(" %d,", vetor[i]);
        }
        System.out.printf(" %d]\n", vetor[vetor.length-1]);

        if(AchaPar.qtd == qtd){
            System.out.printf("%d pares achados\n", qtd);
        }

    }

}
