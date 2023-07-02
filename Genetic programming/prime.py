import math
import time
import random
import numpy as np 
import matplotlib.pyplot as plt
##test function prime from 0 to n && distinct primes
def Is_prime(n):
    if n == 1:
        return False
    if n == 2:
        return True
    if n > 2 and n % 2 ==0:## if it's even not 2, not prime
        return False 
    m_div = math.floor(math.sqrt(n))
    for d in range(3, 1+ m_div, 2):
        if n % d == 0:
            return False
    return True
def findPrime(N):##using Sieve of Eratosthenes to find 1-N all primes
    prime = [True for i in range(N+1)]
    s = 2
    while(s*s <= N):
        if(prime[s] == True):
            for i in range(s*2 , N+1, s):
                prime[i] = False
        s += 1
    prime[0] = False
    prime[1] = False

    primes = []
    for i in range(N+1):
        if(prime[i]):
            primes.append(i)

    return primes
def Fitness(func):
    #return prime from 0 to n, and num of distinct primes 
    n =0; dis_primes = 0
    for i in range(200):
        p = i*i + i*func[0]+func[1]
        
        if(p > 0 and Is_prime(p)):
            dis_primes += 1
            if(n==i):
                n+=1
            else:
                break
    return n, dis_primes
def calculate_fitness(population):
    score = []
    for pop in population:
        n, distinct_primes = Fitness(pop)
        score.append(n)
    return score
def crossover(p):
    """
    The new offspring will have its 
    first half of its genes taken from the first parent.
    second half of its genes taken from the second parent.
    """
    offspring = np.empty((8,2))
    half = 1
    for k in range(8):
        p1 = k%p.shape[0]
        p2 = (k+1)%p.shape[0]

        offspring[k, 0:half]= p[p1, 0:half]
        offspring[k, half:] = p[p2, half:]

    return offspring

def generate_population(nb):
    
    return np.random.randint(low=-200, high=200, size=(nb,2))
def select_parents(current_pop, fitscore, num_parents):
    parents = np.empty((num_parents, 2))

    for i in range(num_parents):
        idx = np.where(fitscore == np.max(fitscore))
        idx = idx[0][0]
        parents[i] = current_pop[idx]
        fitscore[idx] = -math.inf
    return parents


def mutation(offspring, rate = 0.5):
    mut = [i for i in range(0, 2)]
    num = math.floor(rate* 2)
    
    for i in range(offspring.shape[0]):
        pos = random.sample(mut, num)
        v = np.random.randint(low = -200, high=200)
        offspring[i, pos] = offspring[i, pos] + v
    return offspring
def checking(fitness, mg):
    if(np.max(fitness) > 30):##find the solution
        return True, 1
    else:
        if mg >= 5000:##reach max generations but no optimal 
            return True, 0
        else:
            return False, 0
    

"""
##ab bound -200 to 200 (M. Trott, pers. comm.).
y = x^2 +- ax +- b
fitness func: prime from 0 to n	distinct primes
"""
def main():
    pop_size = 10
    selection_size = 2##0.2 rate
    optimal_sol = []
    cur_population = generate_population(pop_size)
    stop = False
    find = 0
    step = 1
    print(cur_population)
    while stop is False:
        print("Generation:", step)
        step+=1
        #evaluate fitness 
        fitness = calculate_fitness(cur_population)
        #print(max(fitness))#store best result in cuurent generation
        #select best parents for mating
        parents = select_parents(cur_population, fitness, selection_size)
        optimal_sol.append(np.max(fitness))
        #use crossover for next generation
        offsprings = crossover(parents)
        mut_offsprings = mutation(offsprings, .5)

        cur_population[0:2, :] = parents
        cur_population[2:,:] = mut_offsprings
        #adding var to offspring using mutation
        stop, find = checking(fitness, step)
        if(stop==True and find == 0):
            print("Renew")
            cur_population = generate_population(pop_size)
            step = 1
            stop = False
        #assign new population 
    fitness = calculate_fitness(cur_population)
    #idx = np.where(fitness == np.max(fitness))



    for i in range(cur_population.shape[0]):
        print("Optimal_coef", cur_population[i])
        print("0-n range: ",fitness[i])
    """
    plt.plot(optimal_sol)
    plt.xlabel("Iteration")
    plt.ylabel("Fitness")
    plt.show()
    """
    #t0 = time.time() testing time
    #t1 = time.time()


if __name__ == '__main__':
    main()